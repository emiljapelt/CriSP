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
var __generator = (this && this.__generator) || function (thisArg, body) {
    var _ = { label: 0, sent: function() { if (t[0] & 1) throw t[1]; return t[1]; }, trys: [], ops: [] }, f, y, t, g;
    return g = { next: verb(0), "throw": verb(1), "return": verb(2) }, typeof Symbol === "function" && (g[Symbol.iterator] = function() { return this; }), g;
    function verb(n) { return function (v) { return step([n, v]); }; }
    function step(op) {
        if (f) throw new TypeError("Generator is already executing.");
        while (_) try {
            if (f = 1, y && (t = op[0] & 2 ? y["return"] : op[0] ? y["throw"] || ((t = y["return"]) && t.call(y), 0) : y.next) && !(t = t.call(y, op[1])).done) return t;
            if (y = 0, t) op = [op[0] & 2, t.value];
            switch (op[0]) {
                case 0: case 1: t = op; break;
                case 4: _.label++; return { value: op[1], done: false };
                case 5: _.label++; y = op[1]; op = [0]; continue;
                case 7: op = _.ops.pop(); _.trys.pop(); continue;
                default:
                    if (!(t = _.trys, t = t.length > 0 && t[t.length - 1]) && (op[0] === 6 || op[0] === 2)) { _ = 0; continue; }
                    if (op[0] === 3 && (!t || (op[1] > t[0] && op[1] < t[3]))) { _.label = op[1]; break; }
                    if (op[0] === 6 && _.label < t[1]) { _.label = t[1]; t = op; break; }
                    if (t && _.label < t[2]) { _.label = t[2]; _.ops.push(op); break; }
                    if (t[2]) _.ops.pop();
                    _.trys.pop(); continue;
            }
            op = body.call(thisArg, _);
        } catch (e) { op = [6, e]; y = 0; } finally { f = t = 0; }
        if (op[0] & 5) throw op[1]; return { value: op[0] ? op[1] : void 0, done: true };
    }
};
exports.__esModule = true;
var child_process_1 = require("child_process");
var fs_1 = require("fs");
var Algorithm;
(function (Algorithm) {
    Algorithm[Algorithm["COUNT_THEN_MOVE"] = 0] = "COUNT_THEN_MOVE";
    Algorithm[Algorithm["CONCURRENT_OUTPUT"] = 1] = "CONCURRENT_OUTPUT";
})(Algorithm || (Algorithm = {}));
var metrics = [
    "cache-misses",
    "L1-dcache-load-misses",
    "L1-icache-load-misses",
    "dTLB-load-misses",
    "iTLB-load-misses",
    "context-switches",
    "branch-misses",
];
function doTheRun(method, thread_count, partition_count) {
    return __awaiter(this, void 0, void 0, function () {
        var regex, processOutput, match, matches, results, _i, matches_1, match_1, split, parsedValue;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0:
                    regex = /((?:\d|\.|,)+)(?:\s{6})(\S+)/gm;
                    return [4 /*yield*/, runProcess("perf", [
                            "stat",
                            "-e",
                            metrics.join(","),
                            "../partitioning.exe",
                            "".concat(method),
                            "".concat(thread_count),
                            "".concat(partition_count),
                        ])];
                case 1:
                    processOutput = (_a.sent());
                    matches = [];
                    while ((match = regex.exec(processOutput)) !== null) {
                        matches.push(match[0]);
                    }
                    results = [];
                    for (_i = 0, matches_1 = matches; _i < matches_1.length; _i++) {
                        match_1 = matches_1[_i];
                        split = match_1.split(/\s+/);
                        parsedValue = Number(split[0].replace(new RegExp("\\.|,", "g"), ""));
                        results.push({ name: split[1], value: parsedValue });
                    }
                    return [2 /*return*/, results];
            }
        });
    });
}
function generateThreadList(upToThreads) {
    var collector = "";
    for (var i = 1; i <= upToThreads; i *= 2) {
        collector += "," + i;
    }
    return collector;
}
function writeToCSV(csvCollectors, method, path) {
    return __awaiter(this, void 0, void 0, function () {
        var collector;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0:
                    if (!!(0, fs_1.existsSync)(path)) return [3 /*break*/, 2];
                    return [4 /*yield*/, mkdirAsync(path)];
                case 1:
                    _a.sent();
                    _a.label = 2;
                case 2: return [4 /*yield*/, mkdirAsync("".concat(path, "/").concat(Algorithm[method]))];
                case 3:
                    _a.sent();
                    for (collector in csvCollectors) {
                        (0, fs_1.writeFileSync)("".concat(path, "/").concat(Algorithm[method], "/").concat(collector, ".csv"), csvCollectors[collector]);
                    }
                    return [2 /*return*/];
            }
        });
    });
}
function runPerfExperiments(method, upToThreads, upToHashbits, path) {
    return __awaiter(this, void 0, void 0, function () {
        var csvCollectors, _i, metrics_1, metric, b, _a, metrics_2, metric, t, results, _b, results_1, result;
        return __generator(this, function (_c) {
            switch (_c.label) {
                case 0:
                    csvCollectors = {};
                    for (_i = 0, metrics_1 = metrics; _i < metrics_1.length; _i++) {
                        metric = metrics_1[_i];
                        csvCollectors[metric] = generateThreadList(upToThreads);
                    }
                    b = 1;
                    _c.label = 1;
                case 1:
                    if (!(b <= upToHashbits)) return [3 /*break*/, 6];
                    for (_a = 0, metrics_2 = metrics; _a < metrics_2.length; _a++) {
                        metric = metrics_2[_a];
                        csvCollectors[metric] += "\n".concat(b);
                    }
                    t = 1;
                    _c.label = 2;
                case 2:
                    if (!(t <= upToThreads)) return [3 /*break*/, 5];
                    return [4 /*yield*/, doTheRun(method, t, Math.pow(2, b))];
                case 3:
                    results = _c.sent();
                    for (_b = 0, results_1 = results; _b < results_1.length; _b++) {
                        result = results_1[_b];
                        csvCollectors[result.name] += "," + result.value;
                    }
                    _c.label = 4;
                case 4:
                    t *= 2;
                    return [3 /*break*/, 2];
                case 5:
                    b++;
                    return [3 /*break*/, 1];
                case 6: return [4 /*yield*/, writeToCSV(csvCollectors, method, path)];
                case 7:
                    _c.sent();
                    return [2 /*return*/];
            }
        });
    });
}
function runProcess(command, args) {
    return new Promise(function (resolve, reject) {
        try {
            var prcs = (0, child_process_1.spawn)(command, args);
            var chunks_1 = [];
            var errorHandler = function (buf) { return reject(buf.toString().trim()); };
            prcs.once("error", errorHandler);
            prcs.stderr.on("data", function (buf) { return chunks_1.push(buf); });
            // prcs.stdout.on("data", (buf) => chunks.push(buf))
            prcs.stdout.on("end", function () {
                resolve(Buffer.concat(chunks_1).toString().trim());
            });
        }
        catch (e) {
            reject(e);
        }
    });
}
function mkdirAsync(path) {
    return new Promise(function (resolve, reject) {
        (0, fs_1.mkdir)(path, function (err) {
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
    return __awaiter(this, void 0, void 0, function () {
        var basePath;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0:
                    basePath = "../benchmark_data/".concat(process.argv[2]);
                    console.log("perf count-then-move");
                    return [4 /*yield*/, runPerfExperiments(Algorithm.COUNT_THEN_MOVE, 32, 18, basePath)];
                case 1:
                    _a.sent();
                    console.log("perf concurrent output");
                    return [4 /*yield*/, runPerfExperiments(Algorithm.CONCURRENT_OUTPUT, 32, 18, basePath)];
                case 2:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    });
}
runAllExperiments().then(function () { return console.log("done"); });
