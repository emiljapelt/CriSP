import { spawn } from "child_process";
import { writeFileSync, mkdir , existsSync} from "fs"

const metrics = [
  "cache-misses",
  "L1-dcache-load-misses",
  "L1-icache-load-misses",
  "dTLB-load-misses",
  "iTLB-load-misses",
  "context-switches",
  "branch-misses",
];

async function doTheRun(
  method,
  thread_count,
  partition_count
) {
  const regex = /((?:\d|\.|,)+)(?:\s{6})(\S+)/gm;
  // maybe necessary to run sudo sysctl -w kernel.perf_event_paranoid=-1
  const processOutput = (await runProcess("perf", [
    "stat",
    "-e",
    metrics.join(","),
    "../partitioning.exe",
    `${method}`,
    `${thread_count}`,
    `${partition_count}`,
  ]));
  let match;

  const matches = []
  while ((match = regex.exec(processOutput)) !== null) {
    matches.push(match[0])
  }
  
  const results = [];
  for (const match of matches) {
    const split = match.split(/\s+/)
    const parsedValue = Number(split[0].replace(new RegExp("\\.|,", "g"), ""));
    results.push({ name: split[1], value: parsedValue });
  }

  return results;
}

function generateThreadList(upToThreads) {
  let collector = ""
  for (let i = 1; i <= upToThreads; i*=2) {
    collector += "," + i
  }
  return collector
}

async function writeToCSV(csvCollectors, method, path) {
  if (!existsSync(path)) await mkdirAsync(path)
  await mkdirAsync(`${path}/${Algorithm[method]}`)

  for (const collector in csvCollectors) {
    writeFileSync(`${path}/${Algorithm[method]}/${collector}.csv`, csvCollectors[collector])
  }
}

async function runPerfExperiments(method, upToThreads, upToHashbits, path) {
  let csvCollectors = {}
  for (const metric of metrics) {
    csvCollectors[metric] = generateThreadList(upToThreads)
  }

  for (let b = 1; b <= upToHashbits; b++) {
    for (const metric of metrics) {
      csvCollectors[metric] += `\n${b}`
    }
    for (let t = 1; t <= upToThreads; t *= 2) {
      const results = await doTheRun(method, t, Math.pow(2, b))
      for (const result of results) {
        csvCollectors[result.name] += "," + result.value
      }
    }
  }

  await writeToCSV(csvCollectors, method, path)
}

function runProcess(command, args) {
  return new Promise((resolve, reject) => {
    try {
      const prcs = spawn(command, args);
      const chunks = [];
      const errorHandler = (buf) => reject(buf.toString().trim());
      prcs.once("error", errorHandler);
      prcs.stderr.on("data", (buf) => chunks.push(buf));
      // prcs.stdout.on("data", (buf) => chunks.push(buf))
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
    mkdir(path, (err) => {
      if (err) {
        reject(err);
      } else {
        resolve();
      }
    });
  });
}

async function runAllExperiments() {
  const basePath = `../benchmark_data/${process.argv[2]}`

  console.log("perf count-then-move")
  await runPerfExperiments(Algorithm.COUNT_THEN_MOVE, 32, 18, basePath)
  console.log("perf concurrent output")
  await runPerfExperiments(Algorithm.CONCURRENT_OUTPUT, 32, 18, basePath)
}

runAllExperiments().then(() => console.log("done"));
