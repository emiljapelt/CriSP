const child_process = require("child_process");
const fs = require("fs");

var Compiler;
(function (Compiler) {
  Compiler[(Compiler["GCC"] = 0)] = "GCC";
  Compiler[(Compiler["CLANG"] = 1)] = "CLANG";
  Compiler[(Compiler["TCC"] = 1)] = "TCC";
})(Compiler || (Compiler = {}));

const compilers = ["gcc", "tcc", "clang"]

const metrics = [
    "instructions",
    "cpu-cycles"
];

function runProcess(command, args) {
    return new Promise((resolve, reject) => {
      try {
        const prcs = child_process.spawn(command, args);
        const chunks = [];
        const errorHandler = (buf) => reject(buf.toString().trim());
        prcs.once("error", errorHandler);
        prcs.stderr.on("data", (buf) => chunks.push(buf));
        prcs.stdout.on("data", (buf) => chunks.push(buf))
        prcs.stdout.on("end", () => {
          resolve(Buffer.concat(chunks).toString().trim());
        });
      } catch (e) {
        reject(e);
      }
    });
  }

async function run_benchmarks(compiler, depth, arity, data_size, repetitions) {
    await runProcess("./bench_compile.sh", [])

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
            const parsedValue = Number(split[0].replace(new RegExp("\\.|,", "g"), ""));
            runResult[split[1]] = parsedValue;
        }
        results.push(runResult);
    }
  
    return results;
}

function calculate_standard_deviation(data, metric) {
    const average = data.reduce((acc, curr) => acc + curr[metric], 0) / data.length;
    const variance = data.map((x) => Math.pow(x[metric] - average, 2)).reduce((a, b) => a + b) / data.length;
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
    const ms_elapsed = data.reduce((acc, curr) => acc + curr["ms-elapsed"], 0) / data.length;
    const instructions = data.reduce((acc, curr) => acc + curr["instructions"], 0) / data.length;
    const cpu_cycles = data.reduce((acc, curr) => acc + curr["cpu-cycles"], 0) / data.length;
    return {"instructions": instructions, "cpu-cycles": cpu_cycles, "ms-elapsed": ms_elapsed};
}

async function main() {
    const tcc_results = await run_benchmarks("tcc", 2, 2, 1000000, 3);
    const standard_deviations = get_run_stddev(tcc_results);
    const averages = get_averages(tcc_results);
    console.log("standard deviation", standard_deviations);
    console.log("averages", averages)
}
main()
