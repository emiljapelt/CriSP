const child_process = require("child_process");
const fs = require("fs");

var Compiler;
(function (Compiler) {
  Compiler[(Compiler["GCC"] = 0)] = "GCC";
  Compiler[(Compiler["CLANG"] = 1)] = "CLANG";
  Compiler[(Compiler["TCC"] = 1)] = "TCC";
})(Compiler || (Compiler = {}));

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
        console.log(processOutput)
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

run_benchmarks("tcc", 2, 2, 1000000, 3).then((results) => {
    console.log(results);
})