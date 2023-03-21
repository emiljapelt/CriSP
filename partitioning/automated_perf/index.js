"use strict";
exports.__esModule = true;
var papaparse_1 = require("papaparse");
var fs_1 = require("fs");
// import { createCanvas } from 'canvas';
// import Chart from 'chart.js';
var csvData = (0, fs_1.readFileSync)("../benchmark_data/server_8passes_concurrent.csv", "utf8");
var results = (0, papaparse_1.parse)(csvData, { header: true });
var data = results.data;
console.log(data);
// const canvas = createCanvas(400, 400);
// const ctx = canvas.getContext('2d');
// const chart = new Chart(ctx, {
//   type: 'bar',
//   data: {
//     labels: [1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18],
//     datasets: [
//       {
//         label: 'My Chart',
//         data: data.map((d) => d.value),
//         backgroundColor: 'rgba(255, 99, 132, 0.2)',
//         borderColor: 'rgba(255, 99, 132, 1)',
//         borderWidth: 1,
//       },
//     ],
//   },
//   options: {
//     scales: {
//       yAxes: [
//         {
//           ticks: {
//             beginAtZero: true,
//           },
//         },
//       ],
//     },
//   },
// });
// const chartImage = canvas.toBuffer('image/png');
