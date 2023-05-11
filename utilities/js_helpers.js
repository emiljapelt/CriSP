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

module.exports = {runProcess, mkdirAsync}