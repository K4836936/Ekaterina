const express = require("express");
const app = express();
const PORT = 3000;

app.use(express.static(__dirname + '/L1'));
app.get("/", (req, res) => {
res.sendFile("L1/Web_Laba1.html", {
root: __dirname
});
});

app.listen(PORT, () => {
console.log("Server is started")
});