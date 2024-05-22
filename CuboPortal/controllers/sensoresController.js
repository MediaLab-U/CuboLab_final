// Importamos el módulo de conexión a la base de datos
const dbConnection = require('./dbConnection');


async function crearSensor(req, res) {
    try {
        const { mac, id_paciente, tipo, bateria, nick, version } = req.body;

        const conn = await dbConnection;

        // Definimos la consulta de inserción
        const insertQuery = `
            INSERT INTO sensores (mac, id_paciente, tipo, bateria, nick, version, fecha, visible)
            VALUES (?, ?, ?, ?, ?, ?, NOW(), TRUE)
        `;
        await conn.execute(insertQuery, [mac, id_paciente, tipo, bateria, nick, version]);

        // Respondemos con un código de estado 201 (Created) y un mensaje de éxito
        res.status(201).json({ message: 'Sensor creado exitosamente' });

    } catch (error) {
        // En caso de error, registramos el error y respondemos con un código de estado 500 (Internal Server Error)
        console.error('Error al crear el sensor', error);
        res.status(500).json({ error: 'Error interno del servidor' });
    }
}


async function sensores(req, res) {
    try {
        const conn = await dbConnection;

        // Definimos la consulta de selección
        const selectQuery = "SELECT * FROM sensores WHERE visible = TRUE";
        const [rows] = await conn.execute(selectQuery);

        // Respondemos con los datos obtenidos
        res.status(200).json(rows);

    } catch (error) {
        // En caso de error, registramos el error y respondemos con un código de estado 500 (Internal Server Error)
        console.error('Error al obtener los sensores', error);
        res.status(500).json({ error: 'Error interno del servidor' });
    }
}


async function sensorID(req, res) {
    try {
        const mac = req.params.mac;

        const conn = await dbConnection;

        // Definimos la consulta de selección
        const selectQuery = "SELECT * FROM sensores WHERE mac = ?";
        const [rows] = await conn.execute(selectQuery, [mac]);

        // Verificamos si se encontró el sensor
        if (rows.length === 0) {
            return res.status(404).json({ message: 'Sensor no encontrado' });
        }

        // Respondemos con la información del sensor
        res.status(200).json(rows[0]);

    } catch (error) {
        // En caso de error, registramos el error y respondemos con un código de estado 500 (Internal Server Error)
        console.error('Error al obtener el sensor', error);
        res.status(500).json({ error: 'Error interno del servidor' });
    }
}




async function actualizarSensor(req, res) {
    try {
        const mac = req.params.mac;
        const { tipo, bateria, nick, version } = req.body;

        const conn = await dbConnection;

        // Construimos la consulta de actualización de manera dinámica
        const fields = [];
        const values = [];
        if (tipo) {
            fields.push('tipo = ?');
            values.push(tipo);
        }
        if (bateria) {
            fields.push('bateria = ?');
            values.push(bateria);
        }
        if (nick) {
            fields.push('nick = ?');
            values.push(nick);
        }
        if (version) {
            fields.push('version = ?');
            values.push(version);
        }
        values.push(mac);

        if (fields.length === 0) {
            return res.status(400).json({ message: 'No se proporcionaron campos para actualizar' });
        }

        const updateQuery = `UPDATE sensores SET ${fields.join(', ')} WHERE mac = ?`;
        const [result] = await conn.execute(updateQuery, values);

        // Verificamos si algún registro fue actualizado
        if (result.affectedRows === 0) {
            return res.status(404).json({ message: 'Sensor no encontrado' });
        }

        // Respondemos con un mensaje de éxito
        res.status(200).json({ message: 'Sensor actualizado exitosamente' });

    } catch (error) {
        // En caso de error, registramos el error y respondemos con un código de estado 500 (Internal Server Error)
        console.error('Error al actualizar el sensor', error);
        res.status(500).json({ error: 'Error interno del servidor' });
    }
}




async function desactivarSensor(req, res) {
    try {
        const mac = req.params.mac;

        const conn = await dbConnection;

        // Definimos la consulta de actualización
        const updateQuery = "UPDATE sensores SET visible = FALSE WHERE mac = ?";
        const [result] = await conn.execute(updateQuery, [mac]);

        // Verificamos si algún registro fue actualizado
        if (result.affectedRows === 0) {
            return res.status(404).json({ message: 'Sensor no encontrado' });
        }

        // Respondemos con un mensaje de éxito
        res.status(200).json({ message: 'Sensor desactivado exitosamente' });

    } catch (error) {
        // En caso de error, registramos el error y respondemos con un código de estado 500 (Internal Server Error)
        console.error('Error al desactivar el sensor', error);
        res.status(500).json({ error: 'Error interno del servidor' });
    }
}



async function crearMedida(req, res) {
    try {
        const mac = req.params.mac;
        const { emocion, bateria } = req.body;

        const conn = await dbConnection;

        // Obtener el id_paciente del sensor definido por la mac
        const selectQuery = "SELECT id_paciente FROM sensores WHERE mac = ?";
        const [rows] = await conn.execute(selectQuery, [mac]);

        if (rows.length === 0) {
            return res.status(404).json({ message: 'Sensor no encontrado' });
        }

        const id_paciente = rows[0].id_paciente;

        // Insertar la nueva medida
        const insertQuery = `
            INSERT INTO medidas (id_paciente, mac, emocion, bateria, fecha, visible)
            VALUES (?, ?, ?, ?, NOW(), TRUE)
        `;
        await conn.execute(insertQuery, [id_paciente, mac, emocion, bateria]);

        // Actualizar el valor de batería del sensor
        const updateQuery = "UPDATE sensores SET bateria = ? WHERE mac = ?";
        await conn.execute(updateQuery, [bateria, mac]);

        // Respondemos con un código de estado 201 (Created) y un mensaje de éxito
        res.status(201).json({ message: 'Medida creada y batería actualizada exitosamente' });

    } catch (error) {
        // En caso de error, registramos el error y respondemos con un código de estado 500 (Internal Server Error)
        console.error('Error al crear la medida', error);
        res.status(500).json({ error: 'Error interno del servidor' });
    }
}


async function medidasSensor(req, res) {
    try {
        const mac = req.params.mac;

        const conn = await dbConnection;

        // Definimos la consulta de selección
        const selectQuery = "SELECT * FROM medidas WHERE mac = ? AND visible = TRUE";
        const [rows] = await conn.execute(selectQuery, [mac]);

        // Respondemos con los datos obtenidos
        res.status(200).json(rows);

    } catch (error) {
        // En caso de error, registramos el error y respondemos con un código de estado 500 (Internal Server Error)
        console.error('Error al obtener las medidas del sensor', error);
        res.status(500).json({ error: 'Error interno del servidor' });
    }
}



// Exportamos las funciones para su uso en otros módulos
module.exports = {
    crearSensor,
    sensores,
    sensorID,
    actualizarSensor,
    desactivarSensor,

    crearMedida,
    medidasSensor
};
