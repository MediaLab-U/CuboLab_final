const bcrypt = require('bcrypt');
const jwt = require('jsonwebtoken');
const dbConnection = require('./dbConnection');

const secretKey = 'cubolab'; // Cambia esto por tu clave secreta

// Función para realizar el inicio de sesión
async function login(req, res) {
    const { email, password, remember } = req.body;

    try {
        const query = 'SELECT * FROM users WHERE email = ?';
        const conn = await dbConnection;
        const [rows] = await conn.execute(query, [email]);
 
        let token = ''

        if (rows.length === 0) {
            return res.status(401).json({ error: 'Credenciales inválidas' });
        }

        const user = rows[0];

        const passwordMatch = await bcrypt.compare(password, user.password);

        if (!passwordMatch) {
            return res.status(401).json({ error: 'Credenciales inválidas' });
        }
        if(remember){
            token = jwt.sign({ userID: user.userID, email: user.email }, secretKey);
        }else{
            token = jwt.sign({ userID: user.userID, email: user.email }, secretKey, { expiresIn: '1h' });
        
        }
        res.json({ token, userID: user.userID, role: user.role, roleID: user.roleID });

    } catch (error) {
        console.error('Error al intentar iniciar sesión:', error);
        res.status(500).json({ error: 'Error interno del servidor' });
    }
}

// Función para crear un nuevo usuario y operador
async function register(req, res) {
    const { email, password, operator, role, pilot} = req.body;

    try {
        const hashedPassword = await bcrypt.hash(password, 10);

        const createUserQuery = "INSERT INTO users (email, password, role) VALUES (?, ?, ?)";
        const conn = await dbConnection;
        const createUserResult = await conn.execute(createUserQuery, [email, hashedPassword, role]);

        const userID = createUserResult[0].insertId;

        // Insertar en la tabla operators con los datos del objeto operator
        if(role == "operator"){
            const { noperator, name, address, cp, city, state, phone } = operator;
        
            const createOperatorQuery = `
                INSERT INTO operators (noperator, name, address, cp, city, state, phone)
                VALUES (?, ?, ?, ?, ?, ?, ?)
            `;
            const createOperatorResult = await conn.execute(createOperatorQuery, [noperator, name, address, cp, city, state, phone]);
    
            const operatorID = createOperatorResult[0].insertId;
    
            // Actualizar el userID con el operatorID
            const updateUserQuery = "UPDATE users SET roleID = ? WHERE userID = ?";
            await conn.execute(updateUserQuery, [operatorID, userID]);
    
            res.status(201).json({ message: 'Nuevo usuario creado exitosamente' });
        }
        if(role == "pilot"){
            
            const insertQuery = "INSERT INTO pilots (nif, name, surname, email, phone, street, number, cp, city, state) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";

            const createUserResult = await conn.execute(insertQuery, [
            pilot.nif,
            pilot.name,
            pilot.surname,
            email,
            pilot.phone,
            pilot.street,
            pilot.number,
            pilot.cp,
            pilot.city,
            pilot.state,
            ]);

            const pilotID = createUserResult[0].insertId;

            const updateUserQuery = "UPDATE users SET roleID = ? WHERE userID = ?";
            await conn.execute(updateUserQuery, [pilotID, userID]);
            
            res.status(201).json({ message: 'Nuevo usuario creado exitosamente' });
    
        }
        
    } catch (error) {
        console.error('Error al crear un nuevo usuario y operador:', error);
        res.status(500).json({ error: 'Error interno del servidor' });
    }
}



module.exports = {
    login,
    register
};
