const child_process = require("child_process");
const fs = require("fs");

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

async function main() {
    const dir = process.argv[2]
    const files = fs.readdirSync(dir).filter(x => x.includes(".exe"))
    if (!fs.existsSync(`${dir}/obj`)) await mkdirAsync(`${dir}/obj`);
    console.log("files", files)
    for (const file of files) {
        const parts = file.split("-")
        console.log(`${dir}/${file}`)
        const res = await runProcess("objdump", ["-d", `${dir}/${file}`])
        const name = parts.length > 3 ? `${parts[1]}-${parts[2]}` : parts[1]
        fs.writeFileSync(`${dir}/obj/${name}.txt`, res)
    }
}

main()