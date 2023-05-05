const child_process = require("child_process");
const fs = require("fs");

const compilers = ["tcc", "clang-default", "clang-O2", "gcc-default", "gcc-O2", "gcc-O3", "gcc-Os"];

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
            `./out/merge-${compiler}-bench.exe`,
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

async function run_combinations(max_depth, arity, data_size, repetitions, compiler) {
    const combinations = []
    for (let depth = 0; depth <= max_depth; depth++) {
        console.log(depth)
        const tcc_results = await run_benchmarks(
            compiler,
            depth,
            arity,
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
    const max_depth = 5;
    const data_size = 10000000;
    const repetitions = 20;
    const arity = 4;

    await runProcess("./bench_compile.sh", []);
    const basePath = "../benchmark_data";
    if (!fs.existsSync(basePath)) await mkdirAsync(basePath);
    const now = getDateString()
    const dir = `${basePath}/${now}_arity-${arity}`

    await mkdirAsync(dir)


    const combination_results = []
    for (const compiler of compilers) {
        console.log(compiler)
        const result = await run_combinations(max_depth, arity, data_size, repetitions, compiler)
        combination_results.push(result)
    }

    for (let type of ["standard_deviations", "averages"]) {
        const typeDir = `${dir}/${type}`
        if (!fs.existsSync(typeDir)) await mkdirAsync(typeDir)
        for (let metric of [...metrics, "ms-elapsed"]) {
            const csvCollector = [["",...compilers].join(",")]
            for (let i = 0; i <= max_depth; i++) {
                let stringBuilder = [i]
                for (let comp = 0; comp < compilers.length; comp++) {
                    stringBuilder.push(`${combination_results[comp][i][type][metric]}`)
                }
                csvCollector.push(stringBuilder.join(","))
            }
            fs.writeFileSync(`${typeDir}/${metric}.csv`, csvCollector.join("\n"))
        }

        const csvCollector = [["",...compilers].join(",")]
        for (let i = 0; i <= max_depth; i++) {
            let stringBuilder = [i]
            for (let comp = 0; comp < compilers.length; comp++) {
                stringBuilder.push(`${combination_results[comp][i][type]["instructions"] / combination_results[comp][i][type]["cpu-cycles"]}`)
            }
            csvCollector.push(stringBuilder.join(","))
        }
        fs.writeFileSync(`${typeDir}/instrutions_per_clock.csv`, csvCollector.join("\n"))
    }
}

main();
