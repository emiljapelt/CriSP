const child_process = require("child_process");
const fs = require("fs");

var Compiler;
(function (Compiler) {
    Compiler[(Compiler["GCC"] = 0)] = "GCC";
    Compiler[(Compiler["CLANG"] = 1)] = "CLANG";
    Compiler[(Compiler["TCC"] = 1)] = "TCC";
})(Compiler || (Compiler = {}));

const compilers = ["gcc", "tcc", "clang"];

const metrics = ["instructions", "cpu-cycles"];

function runProcess(command, args) {
    return new Promise((resolve, reject) => {
        try {
            const prcs = child_process.spawn(command, args);
            const chunks = [];
            const errorHandler = (buf) => reject(buf.toString().trim());
            prcs.once("error", errorHandler);
            prcs.stderr.on("data", (buf) => chunks.push(buf));
            prcs.stdout.on("data", (buf) => chunks.push(buf));
            prcs.stdout.on("end", () => {
                resolve(Buffer.concat(chunks).toString().trim());
            });
        } catch (e) {
            reject(e);
        }
    });
}

function mkdirAsync(path) {
    return new Promise((resolve, reject) => {
        fs.mkdir(path, (err) => {
            if (err) {
                reject(err);
            } else {
                resolve();
            }
        });
    });
}

async function run_benchmarks(compiler, depth, arity, data_size, repetitions) {
    const results = [];
    const regex = /((?:\d|\.|,)+)(?:\s{6})(\S+)/gm;
    // maybe necessary to run sudo sysctl -w kernel.perf_event_paranoid=-1
    for (let i = 0; i < repetitions; i++) {
        const processOutput = await runProcess("perf", [
            "stat",
            "-e",
            metrics.join(","),
            `./merge-${compiler}-bench.exe`,
            `${depth}`,
            `${arity}`,
            `${data_size}`,
        ]);
        let match;
        const matches = [];
        while ((match = regex.exec(processOutput)) !== null) {
            matches.push(match[0]);
        }

        const runResult = {};
        for (const match of matches) {
            const split = match.split(/\s+/);
            const parsedValue = Number(
                split[0].replace(new RegExp("\\.|,", "g"), "")
            );
            runResult[split[1]] = parsedValue;
        }
        results.push(runResult);
    }

    return results;
}

function calculate_standard_deviation(data, metric) {
    const average =
        data.reduce((acc, curr) => acc + curr[metric], 0) / data.length;
    const variance =
        data.map((x) => Math.pow(x[metric] - average, 2)).reduce((a, b) => a + b) /
        data.length;
    return Math.sqrt(variance);
}

function get_run_stddev(data) {
    const res = {};
    for (const metric of [...metrics, "ms-elapsed"]) {
        res[metric] = calculate_standard_deviation(data, metric);
    }
    return res;
}

function get_averages(data) {
    const ms_elapsed =
        data.reduce((acc, curr) => acc + curr["ms-elapsed"], 0) / data.length;
    const instructions =
        data.reduce((acc, curr) => acc + curr["instructions"], 0) / data.length;
    const cpu_cycles =
        data.reduce((acc, curr) => acc + curr["cpu-cycles"], 0) / data.length;
    return {
        instructions: instructions,
        "cpu-cycles": cpu_cycles,
        "ms-elapsed": ms_elapsed,
    };
}

async function run_combinations(max_depth, data_size, repetitions, metric) {
    const combinations = []
    for (let depth = 0; depth <= max_depth; depth++) {
        const tcc_results = await run_benchmarks(
            metric,
            depth,
            2,
            data_size,
            repetitions
        );
        const standard_deviations = get_run_stddev(tcc_results);
        const averages = get_averages(tcc_results);
        combinations.push({standard_deviations, averages})
    }
    return combinations;
}

function getDateString() {
    const date = new Date()
    return `${date.getDate()}-${date.getMonth() + 1}-${date.getFullYear()}_${date.getHours()}-${date.getMinutes()}`
}

async function main() {
    await runProcess("./bench_compile.sh", []);
    const basePath = "./benchmark_data";
    if (!fs.existsSync(basePath)) await mkdirAsync(basePath);
    const now = getDateString()
    const dir = basePath + "/" + now

    await mkdirAsync(dir)

    const max_depth = 4;
    const data_size = 10000000;
    const repetitions = 20;
    const tcc_combinations = await run_combinations(max_depth, data_size, repetitions, "tcc");
    const gcc_combinations = await run_combinations(max_depth, data_size, repetitions, "gcc");
    const clang_combinations = await run_combinations(max_depth, data_size, repetitions, "clang");
    console.log(tcc_combinations)
    
    for (let type of ["standard_deviations", "averages"]) {
        const typeDir = `${dir}/${type}`
        if (!fs.existsSync(typeDir)) await mkdirAsync(typeDir)
        for (let metric of [...metrics, "ms-elapsed"]) {
            const csvCollector = [",tcc,gcc,clang"]
            for (let i = 0; i <= max_depth; i++) {
                csvCollector.push(`${i},${tcc_combinations[i][type][metric]},${gcc_combinations[i][type][metric]},${clang_combinations[i][type][metric]}`)
            }
            fs.writeFileSync(`${typeDir}/${metric}.csv`, csvCollector.join("\n"))
        }
    }
}

main();
