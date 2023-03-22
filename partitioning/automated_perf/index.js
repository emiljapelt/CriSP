"use strict";
var __awaiter = (this && this.__awaiter) || function (thisArg, _arguments, P, generator) {
    function adopt(value) { return value instanceof P ? value : new P(function (resolve) { resolve(value); }); }
    return new (P || (P = Promise))(function (resolve, reject) {
        function fulfilled(value) { try { step(generator.next(value)); } catch (e) { reject(e); } }
        function rejected(value) { try { step(generator["throw"](value)); } catch (e) { reject(e); } }
        function step(result) { result.done ? resolve(result.value) : adopt(result.value).then(fulfilled, rejected); }
        step((generator = generator.apply(thisArg, _arguments || [])).next());
    });
};
Object.defineProperty(exports, "__esModule", { value: true });
const child_process_1 = require("child_process");
const fs_1 = require("fs");
var Algorithm;
(function (Algorithm) {
    Algorithm[Algorithm["COUNT_THEN_MOVE"] = 0] = "COUNT_THEN_MOVE";
    Algorithm[Algorithm["CONCURRENT_OUTPUT"] = 1] = "CONCURRENT_OUTPUT";
})(Algorithm || (Algorithm = {}));
const metrics = [
    "cache-misses",
    "L1-dcache-load-misses",
    "L1-icache-load-misses",
    "dTLB-load-misses",
    "iTLB-load-misses",
    "context-switches",
    "branch-misses",
];
function doTheRun(method, thread_count, partition_count) {
    return __awaiter(this, void 0, void 0, function* () {
        const regex = /(?<value>(?:\d|\.)+)(?:\s{6})(?<name>\S+)/gm;
        // maybe necessary to run sudo sysctl -w kernel.perf_event_paranoid=-1
        const processOutput = (yield runProcess("perf", [
            "stat",
            "-e",
            metrics.join(","),
            "../partitioning.exe",
            `${method}`,
            `${thread_count}`,
            `${partition_count}`,
        ]));
        const matches = processOutput.matchAll(regex);
        const results = [];
        for (let next = matches.next(); next.value; next = matches.next()) {
            const parsedValue = Number(next.value[1].replaceAll(".", ""));
            results.push({ name: next.value[2], value: parsedValue });
        }
        return results;
    });
}
function generateThreadList(upToThreads) {
    let collector = "";
    for (let i = 1; i <= upToThreads; i *= 2) {
        collector += "," + i;
    }
    return collector;
}
function writeToCSV(csvCollectors, method, path) {
    return __awaiter(this, void 0, void 0, function* () {
        if (!(0, fs_1.existsSync)(path))
            yield mkdirAsync(path);
        yield mkdirAsync(`${path}/${Algorithm[method]}`);
        for (const collector in csvCollectors) {
            (0, fs_1.writeFileSync)(`${path}/${Algorithm[method]}/${collector}.csv`, csvCollectors[collector]);
        }
    });
}
function runPerfExperiments(method, upToThreads, upToHashbits, path) {
    return __awaiter(this, void 0, void 0, function* () {
        let csvCollectors = {};
        for (const metric of metrics) {
            csvCollectors[metric] = generateThreadList(upToThreads);
        }
        for (let b = 1; b <= upToHashbits; b++) {
            for (const metric of metrics) {
                csvCollectors[metric] += `\n${b}`;
            }
            for (let t = 1; t <= upToThreads; t++) {
                const results = yield doTheRun(method, t, Math.pow(2, b));
                for (const result of results) {
                    csvCollectors[result.name] += "," + result.value;
                }
            }
        }
        yield writeToCSV(csvCollectors, method, path);
    });
}
function runProcess(command, args) {
    return new Promise((resolve, reject) => {
        try {
            const prcs = (0, child_process_1.spawn)(command, args);
            const chunks = [];
            const errorHandler = (buf) => reject(buf.toString().trim());
            prcs.once("error", errorHandler);
            prcs.stderr.on("data", (buf) => chunks.push(buf));
            // prcs.stdout.on("data", (buf) => chunks.push(buf))
            prcs.stdout.on("end", () => {
                resolve(Buffer.concat(chunks).toString().trim());
            });
        }
        catch (e) {
            reject(e);
        }
    });
}
function mkdirAsync(path) {
    return new Promise((resolve, reject) => {
        (0, fs_1.mkdir)(path, (err) => {
            if (err) {
                reject(err);
            }
            else {
                resolve();
            }
        });
    });
}
function runAllExperiments() {
    return __awaiter(this, void 0, void 0, function* () {
        const basePath = `../benchmark_data/${process.argv[2]}`;
        console.log("perf count-then-move");
        yield runPerfExperiments(Algorithm.COUNT_THEN_MOVE, 2, 4, basePath);
        console.log("perf concurrent output");
        yield runPerfExperiments(Algorithm.CONCURRENT_OUTPUT, 2, 4, basePath);
    });
}
runAllExperiments().then(() => console.log("done"));
