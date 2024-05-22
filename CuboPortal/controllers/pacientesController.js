// Importamos el módulo de conexión a la base de datos
const dbConnection = require('./dbConnection');


async function crearPaciente(req, res) {
    try {
        const { id_centro, nombre } = req.body;

        const conn = await dbConnection;

        // Definimos la consulta de inserción
        const insertQuery = `
            INSERT INTO pacientes (id_centro, nombre, fecha, visible)
            VALUES (?, ?, NOW(), TRUE)
        `;
        await conn.execute(insertQuery, [id_centro, nombre]);

        // Respondemos con un código de estado 201 (Created) y un mensaje de éxito
        res.status(201).json({ message: 'Paciente creado exitosamente' });

    } catch (error) {
        // En caso de error, registramos el error y respondemos con un código de estado 500 (Internal Server Error)
        console.error('Error al crear el paciente', error);
        res.status(500).json({ error: 'Error interno del servidor' });
    }
}

async function pacientes(req, res) {
    try {
        const conn = await dbConnection;

        // Definimos la consulta de selección
        const selectQuery = "SELECT * FROM pacientes WHERE visible = TRUE";
        const [rows] = await conn.execute(selectQuery);

        // Respondemos con los datos obtenidos
        res.status(200).json(rows);

    } catch (error) {
        // En caso de error, registramos el error y respondemos con un código de estado 500 (Internal Server Error)
        console.error('Error al obtener los pacientes', error);
        res.status(500).json({ error: 'Error interno del servidor' });
    }
}

async function pacienteID(req, res) {
    try {
        const id_paciente = req.params.id_paciente;

        const conn = await dbConnection;

        // Definimos la consulta de selección
        const selectQuery = "SELECT * FROM pacientes WHERE id_paciente = ?";
        const [rows] = await conn.execute(selectQuery, [id_paciente]);

        // Verificamos si se encontró el paciente
        if (rows.length === 0) {
            return res.status(404).json({ message: 'Paciente no encontrado' });
        }

        // Respondemos con la información del paciente
        res.status(200).json(rows[0]);

    } catch (error) {
        // En caso de error, registramos el error y respondemos con un código de estado 500 (Internal Server Error)
        console.error('Error al obtener el paciente', error);
        res.status(500).json({ error: 'Error interno del servidor' });
    }
}



async function actualizarPaciente(req, res) {
    try {
        const id_paciente = req.params.id_paciente;
        const { id_centro, nombre } = req.body;

        const conn = await dbConnection;

        // Construimos la consulta de actualización de manera dinámica
        const fields = [];
        const values = [];
        if (id_centro) {
            fields.push('id_centro = ?');
            values.push(id_centro);
        }
        if (nombre) {
            fields.push('nombre = ?');
            values.push(nombre);
        }
        values.push(id_paciente);

        if (fields.length === 0) {
            return res.status(400).json({ message: 'No se proporcionaron campos para actualizar' });
        }

        const updateQuery = `UPDATE pacientes SET ${fields.join(', ')} WHERE id_paciente = ?`;
        const [result] = await conn.execute(updateQuery, values);

        // Verificamos si algún registro fue actualizado
        if (result.affectedRows === 0) {
            return res.status(404).json({ message: 'Paciente no encontrado' });
        }

        // Respondemos con un mensaje de éxito
        res.status(200).json({ message: 'Paciente actualizado exitosamente' });

    } catch (error) {
        // En caso de error, registramos el error y respondemos con un código de estado 500 (Internal Server Error)
        console.error('Error al actualizar el paciente', error);
        res.status(500).json({ error: 'Error interno del servidor' });
    }
}



async function desactivarPaciente(req, res) {
    try {
        const id_paciente = req.params.id_paciente;

        const conn = await dbConnection;

        // Definimos la consulta de actualización
        const updateQuery = "UPDATE pacientes SET visible = FALSE WHERE id_paciente = ?";
        const [result] = await conn.execute(updateQuery, [id_paciente]);

        // Verificamos si algún registro fue actualizado
        if (result.affectedRows === 0) {
            return res.status(404).json({ message: 'Paciente no encontrado' });
        }

        // Respondemos con un mensaje de éxito
        res.status(200).json({ message: 'Paciente desactivado exitosamente' });

    } catch (error) {
        // En caso de error, registramos el error y respondemos con un código de estado 500 (Internal Server Error)
        console.error('Error al desactivar el paciente', error);
        res.status(500).json({ error: 'Error interno del servidor' });
    }
}



async function pacientesCentro(req, res) {
    try {
        const id_centro = req.params.id_centro;

        const conn = await dbConnection;

        // Definimos la consulta de selección
        const selectQuery = "SELECT * FROM pacientes WHERE id_centro = ? AND visible = TRUE";
        const [rows] = await conn.execute(selectQuery, [id_centro]);

        // Respondemos con los datos obtenidos
        res.status(200).json(rows);

    } catch (error) {
        // En caso de error, registramos el error y respondemos con un código de estado 500 (Internal Server Error)
        console.error('Error al obtener los pacientes por centro', error);
        res.status(500).json({ error: 'Error interno del servidor' });
    }
}

async function usuariosPaciente(req, res) {
    try {
        const id_paciente = req.params.id_paciente;

        const conn = await dbConnection;

        // Definimos la consulta de selección
        const selectQuery = `
            SELECT u.id_usuario, u.nombre 
            FROM pacientes_usuarios pu
            JOIN usuarios u ON pu.id_usuario = u.id_usuario
            WHERE pu.id_paciente = ?
        `;
        const [rows] = await conn.execute(selectQuery, [id_paciente]);

        // Respondemos con los datos obtenidos
        res.status(200).json(rows);

    } catch (error) {
        // En caso de error, registramos el error y respondemos con un código de estado 500 (Internal Server Error)
        console.error('Error al obtener los usuarios del paciente', error);
        res.status(500).json({ error: 'Error interno del servidor' });
    }
}


async function medidasPaciente(req, res) {
    try {
        const id_paciente = req.params.id_paciente;

        const conn = await dbConnection;

        // Definimos la consulta de selección
        const selectQuery = "SELECT * FROM medidas WHERE id_paciente = ? AND visible = TRUE";
        const [rows] = await conn.execute(selectQuery, [id_paciente]);

        // Respondemos con los datos obtenidos
        res.status(200).json(rows);

    } catch (error) {
        // En caso de error, registramos el error y respondemos con un código de estado 500 (Internal Server Error)
        console.error('Error al obtener las medidas del paciente', error);
        res.status(500).json({ error: 'Error interno del servidor' });
    }
}





// Exportamos las funciones para su uso en otros módulos
module.exports = {
    crearPaciente,
    pacientes,
    pacienteID,
    actualizarPaciente,
    desactivarPaciente,
    pacientesCentro,
    usuariosPaciente,
    medidasPaciente
};
