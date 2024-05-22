const mysql = require('mysql2/promise');

const dbConfig = {
    host: '85.31.236.104',
    user: 'rogo',
    password: 'rogo23k',
    database: 'cubolab'
}

const connection = mysql.createConnection(dbConfig);

module.exports = connection
