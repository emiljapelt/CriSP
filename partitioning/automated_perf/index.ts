import { spawn } from "child_process";
import { writeFileSync } from "fs"

interface tuple {
  name: string;
  value: number;
}

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
  method: number,
  thread_count: number,
  partition_count: number
) {
  const regex = /(?<value>(?:\d|\.)+)(?:\s{6})(?<name>\S+)/gm;
  // maybe necessary to run sudo sysctl -w kernel.perf_event_paranoid=-1
  const processOutput = (await runProcess("perf", [
    "stat",
    "-e",
    metrics.join(","),
    "../partitioning.exe",
    `${method}`,
    `${thread_count}`,
    `${partition_count}`,
  ])) as string;

  const matches = processOutput.matchAll(regex);
  const results: tuple[] = [];
  let next = matches.next();
  while (next.value) {
    const parsedValue = Number(next.value[1].replaceAll(".", ""));
    results.push({ name: next.value[2], value: parsedValue });
    next = matches.next();
  }

  console.log(results);

  return results;
}

function generateThreadList(upToThreads: number) {
  let collector = ""
  for (let i = 1; i <= upToThreads; i*=2) {
    collector += "," + i
  }
  return collector
}

async function generateCSV(method: number, upToThreads: number, upToHashbits: number) {
  let csvCollectors: {[key: string]: string} = {}
  for (const metric of metrics) {
    csvCollectors[metric] = generateThreadList(upToThreads)
  }

  for (let b = 1; b <= upToHashbits; b++) {
    for (const metric of metrics) {
      csvCollectors[metric] += `\n${b}`
    }
    for (let t = 1; t <= upToThreads; t++) {
      const results = await doTheRun(method, t, Math.pow(2, b))
      for (const result of results) {
        csvCollectors[result.name] += "," + result.value
      }
    }
  }

  for (const collector in csvCollectors) {
    writeFileSync(`./meme/${collector}.csv`, csvCollectors[collector])
  }
}


// doTheRun(0, 16, 64);
generateCSV(0, 2, 4)

function runProcess(command: string, args: string[]) {
  return new Promise((resolve, reject) => {
    try {
      const prcs = spawn(command, args);
      const chunks: Uint8Array[] = [];
      const errorHandler = (buf: Error): void => reject(buf.toString().trim());
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
