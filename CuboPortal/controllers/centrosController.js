// Importamos el módulo de conexión a la base de datos
const dbConnection = require('./dbConnection');


async function crearCentro(req, res) {
    try {
        const { nombre } = req.body;
        
        const conn = await dbConnection;

        // Definimos la consulta de inserción
        const insertQuery = "INSERT INTO centros (nombre, fecha, visible) VALUES (?, NOW(), TRUE)";
        await conn.execute(insertQuery, [nombre]);

        // Respondemos con un código de estado 201 (Created) y un mensaje de éxito
        res.status(201).json({ message: 'Centro creado exitosamente' });

    } catch (error) {
        // En caso de error, registramos el error y respondemos con un código de estado 500 (Internal Server Error)
        console.error('Error al crear el centro', error);
        res.status(500).json({ error: 'Error interno del servidor' });
    }
}

async function actualizarCentro(req, res) {
    try {
        const id_centro = req.params.id_centro;
        const { nombre } = req.body;

        const conn = await dbConnection;

        // Definimos la consulta de actualización
        const updateQuery = "UPDATE centros SET nombre = ? WHERE id_centro = ?";
        const [result] = await conn.execute(updateQuery, [nombre, id_centro]);

        // Verificamos si algún registro fue actualizado
        if (result.affectedRows === 0) {
            return res.status(404).json({ message: 'Centro no encontrado' });
        }

        // Respondemos con un mensaje de éxito
        res.status(200).json({ message: 'Nombre del centro actualizado exitosamente' });

    } catch (error) {
        // En caso de error, registramos el error y respondemos con un código de estado 500 (Internal Server Error)
        console.error('Error al actualizar el nombre del centro', error);
        res.status(500).json({ error: 'Error interno del servidor' });
    }
}


async function desactivarCentro(req, res) {
    try {
        const id_centro = req.params.id_centro;

        const conn = await dbConnection;

        // Definimos la consulta de actualización
        const updateQuery = "UPDATE centros SET visible = FALSE WHERE id_centro = ?";
        const [result] = await conn.execute(updateQuery, [id_centro]);

        // Verificamos si algún registro fue actualizado
        if (result.affectedRows === 0) {
            return res.status(404).json({ message: 'Centro no encontrado' });
        }

        // Respondemos con un mensaje de éxito
        res.status(200).json({ message: 'Centro marcado como no visible exitosamente' });

    } catch (error) {
        // En caso de error, registramos el error y respondemos con un código de estado 500 (Internal Server Error)
        console.error('Error al marcar el centro como no visible', error);
        res.status(500).json({ error: 'Error interno del servidor' });
    }
}


async function centros(req, res) {
    try {
        const conn = await dbConnection;

        // Definimos la consulta de selección
        const selectQuery = "SELECT * FROM centros WHERE visible = TRUE";
        const [rows] = await conn.execute(selectQuery);

        // Respondemos con los datos obtenidos
        res.status(200).json(rows);

    } catch (error) {
        // En caso de error, registramos el error y respondemos con un código de estado 500 (Internal Server Error)
        console.error('Error al obtener los centros', error);
        res.status(500).json({ error: 'Error interno del servidor' });
    }
}

async function centroID(req, res) {
    try {
        const id_centro = req.params.id_centro;

        const conn = await dbConnection;

        // Definimos la consulta de selección
        const selectQuery = "SELECT * FROM centros WHERE id_centro = ?";
        const [rows] = await conn.execute(selectQuery, [id_centro]);

        // Verificamos si se encontró el centro
        if (rows.length === 0) {
            return res.status(404).json({ message: 'Centro no encontrado' });
        }

        // Respondemos con la información del centro
        res.status(200).json(rows[0]);

    } catch (error) {
        // En caso de error, registramos el error y respondemos con un código de estado 500 (Internal Server Error)
        console.error('Error al obtener el centro', error);
        res.status(500).json({ error: 'Error interno del servidor' });
    }
}



// Exportamos las funciones para su uso en otros módulos
module.exports = {
    crearCentro,
    actualizarCentro,
    desactivarCentro,
    centros,
    centroID
};
