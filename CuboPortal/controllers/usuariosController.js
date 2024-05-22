// Importamos el módulo de conexión a la base de datos
const dbConnection = require('./dbConnection');

async function crearUsuario(req, res) {
    try {
        const { email, contrasena, nombre, id_centro } = req.body;
        
        const conn = await dbConnection;

        // Definimos la consulta de inserción
        const insertQuery = `
            INSERT INTO usuarios (email, contrasena, nombre, id_centro, fecha, visible) 
            VALUES (?, ?, ?, ?, NOW(), TRUE)
        `;
        await conn.execute(insertQuery, [email, contrasena, nombre, id_centro]);

        // Respondemos con un código de estado 201 (Created) y un mensaje de éxito
        res.status(201).json({ message: 'Usuario creado exitosamente' });

    } catch (error) {
        // En caso de error, registramos el error y respondemos con un código de estado 500 (Internal Server Error)
        console.error('Error al crear el usuario', error);
        res.status(500).json({ error: 'Error interno del servidor' });
    }
}

async function usuarios(req, res) {
    try {
        const conn = await dbConnection;

        // Definimos la consulta de selección con la cláusula WHERE para filtrar por visible = 1
        const selectQuery = "SELECT * FROM usuarios WHERE visible = TRUE";
        const [rows] = await conn.execute(selectQuery);

        // Respondemos con los datos obtenidos
        res.status(200).json(rows);

    } catch (error) {
        // En caso de error, registramos el error y respondemos con un código de estado 500 (Internal Server Error)
        console.error('Error al obtener los usuarios', error);
        res.status(500).json({ error: 'Error interno del servidor' });
    }
}


async function usuarioID(req, res) {
    try {
        const id_usuario = req.params.id_usuario;

        const conn = await dbConnection;

        // Definimos la consulta de selección
        const selectQuery = "SELECT * FROM usuarios WHERE id_usuario = ?";
        const [rows] = await conn.execute(selectQuery, [id_usuario]);

        // Verificamos si se encontró el usuario
        if (rows.length === 0) {
            return res.status(404).json({ message: 'Usuario no encontrado' });
        }

        // Respondemos con la información del usuario
        res.status(200).json(rows[0]);

    } catch (error) {
        // En caso de error, registramos el error y respondemos con un código de estado 500 (Internal Server Error)
        console.error('Error al obtener el usuario', error);
        res.status(500).json({ error: 'Error interno del servidor' });
    }
}



async function actualizarUsuario(req, res) {
    try {
        const id_usuario = req.params.id_usuario;
        const { email, contrasena, nombre, id_centro } = req.body;

        const conn = await dbConnection;

        // Construimos la consulta de actualización de manera dinámica
        const fields = [];
        const values = [];
        if (email) {
            fields.push('email = ?');
            values.push(email);
        }
        if (contrasena) {
            fields.push('contrasena = ?');
            values.push(contrasena);
        }
        if (nombre) {
            fields.push('nombre = ?');
            values.push(nombre);
        }
        if (id_centro) {
            fields.push('id_centro = ?');
            values.push(id_centro);
        }
        values.push(id_usuario);

        if (fields.length === 0) {
            return res.status(400).json({ message: 'No se proporcionaron campos para actualizar' });
        }

        const updateQuery = `UPDATE usuarios SET ${fields.join(', ')} WHERE id_usuario = ?`;
        const [result] = await conn.execute(updateQuery, values);

        // Verificamos si algún registro fue actualizado
        if (result.affectedRows === 0) {
            return res.status(404).json({ message: 'Usuario no encontrado' });
        }

        // Respondemos con un mensaje de éxito
        res.status(200).json({ message: 'Usuario actualizado exitosamente' });

    } catch (error) {
        // En caso de error, registramos el error y respondemos con un código de estado 500 (Internal Server Error)
        console.error('Error al actualizar el usuario', error);
        res.status(500).json({ error: 'Error interno del servidor' });
    }
}




async function usuariosCentro(req, res) {
    try {
        const id_centro = req.params.id_centro;

        const conn = await dbConnection;

        // Definimos la consulta de selección
        const selectQuery = "SELECT * FROM usuarios WHERE id_centro = ?";
        const [rows] = await conn.execute(selectQuery, [id_centro]);

        // Respondemos con los datos obtenidos
        res.status(200).json(rows);

    } catch (error) {
        // En caso de error, registramos el error y respondemos con un código de estado 500 (Internal Server Error)
        console.error('Error al obtener los usuarios por centro', error);
        res.status(500).json({ error: 'Error interno del servidor' });
    }
}

async function desactivarUsuario(req, res) {
    try {
        const id_usuario = req.params.id_usuario;

        const conn = await dbConnection;

        // Definimos la consulta de actualización
        const updateQuery = "UPDATE usuarios SET visible = FALSE WHERE id_usuario = ?";
        const [result] = await conn.execute(updateQuery, [id_usuario]);

        // Verificamos si algún registro fue actualizado
        if (result.affectedRows === 0) {
            return res.status(404).json({ message: 'Usuario no encontrado' });
        }

        // Respondemos con un mensaje de éxito
        res.status(200).json({ message: 'Usuario desactivado exitosamente' });

    } catch (error) {
        // En caso de error, registramos el error y respondemos con un código de estado 500 (Internal Server Error)
        console.error('Error al desactivar el usuario', error);
        res.status(500).json({ error: 'Error interno del servidor' });
    }
}

async function pacientesUsuario(req, res) {
    try {
        const id_usuario = req.params.id_usuario;

        const conn = await dbConnection;

        // Definimos la consulta de selección
        const selectQuery = `
            SELECT p.id_paciente, p.nombre AS paciente_nombre, c.nombre AS centro_nombre
            FROM pacientes_usuarios pu
            JOIN pacientes p ON pu.id_paciente = p.id_paciente
            JOIN centros c ON p.id_centro = c.id_centro
            WHERE pu.id_usuario = ?
        `;
        const [rows] = await conn.execute(selectQuery, [id_usuario]);

        // Respondemos con los datos obtenidos
        res.status(200).json(rows);

    } catch (error) {
        // En caso de error, registramos el error y respondemos con un código de estado 500 (Internal Server Error)
        console.error('Error al obtener los pacientes del usuario', error);
        res.status(500).json({ error: 'Error interno del servidor' });
    }
}





// Exportamos las funciones para su uso en otros módulos
module.exports = {
    crearUsuario,
    usuarios,
    usuarioID,
    actualizarUsuario,
    desactivarUsuario,
    usuariosCentro,
    pacientesUsuario
};
