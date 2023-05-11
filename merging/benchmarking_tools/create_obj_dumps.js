const child_process = require("child_process");
const fs = require("fs");
const {mkdirAsync, runProcess} = require("../../utilities/js_helpers")

async function main() {
    const dir = process.argv[2]
    const files = fs.readdirSync(dir).filter(x => x.includes(".exe"))
    if (!fs.existsSync(`${dir}/obj`)) await mkdirAsync(`${dir}/obj`);
    for (const file of files) {
        const parts = file.split("-")
        const res = await runProcess("objdump", ["-d", `${dir}/${file}`])
        const name = parts.length > 3 ? `${parts[1]}-${parts[2]}` : parts[1]
        fs.writeFileSync(`${dir}/obj/${name}.txt`, res)
    }
}

main()