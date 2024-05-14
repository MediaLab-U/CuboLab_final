const express = require('express');
const { networkInterfaces } = require('os');
const fs = require('fs');
const path = require('path');
 
const app = express();
const nets = networkInterfaces();
 
// Server port
const PORT = 3000;
 
let downloadCounter = 1;

app.get('/', (req, res) => {
    const indexPath = path.join(__dirname, '/public/index.html');
    fs.readFile(indexPath, 'utf8', (err, data) => {
        if (err) {
            console.error('Error reading index.html:', err);
            res.status(500).send('Error reading index.html');
            return;
        }
        res.set('Content-Type', 'text/html');
        res.send(data);
    });
});

app.get('/firmware', (request, response) => {
    response.download(path.join(__dirname, 'firmware/cubelab_v_0_1.bin'), 'cubelab_v_0_1.bin', (err)=>{
        if (err) {
            console.error("Problem on download firmware: ", err)
        }else{
            downloadCounter++;
        }
    });
    console.log('Your file has been downloaded '+downloadCounter+' times!')
})


const LAST_VERSION = 0.3;
app.get('/update/:version', (request, response) => {
    const version = (request.params.version) ? parseFloat(request.params.version) : Infinity;
 
    if (version<LAST_VERSION){
        // If you need an update go here
        response.download(path.join(__dirname, 'firmware/cubelab_v_0_1.bin'), 'cubelab_v_0_1.bin', (err)=>{
            if (err) {
                console.error("Problem on download firmware: ", err)
            }else{
                downloadCounter++;
            }
        });
        console.log('Your file has been downloaded '+downloadCounter+' times!')
    }else{
        response.status(304).send('No update needed!')
    }
});
 

app.listen(PORT, () => {
    const results = {}; // Or just '{}', an empty object
 
    for (const name of Object.keys(nets)) {
        for (const net of nets[name]) {
            // Skip over non-IPv4 and internal (i.e. 127.0.0.1) addresses
            if (net.family === 'IPv4' && !net.internal) {
                if (!results[name]) {
                    results[name] = [];
                }
                results[name].push(net.address);
            }
        }
    }
 
    console.log('Listening on port '+PORT+'\n', results)
});