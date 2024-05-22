const express = require('express');
const cors = require('cors');
const http = require('http');
const app = express();

const server = http.createServer(app);


const allowedOrigins = ['http://localhost:8080'];
const authController = require('./controllers/authController');
const centroController = require('./controllers/centrosController');
const usuariosController = require('./controllers/usuariosController');
const pacientesController = require('./controllers/pacientesController');
const sensoresController = require('./controllers/sensoresController');

const corsOptions = {
    origin: function (origin, callback) {
        if (!origin || allowedOrigins.includes(origin)) {
            callback(null, true);
        } else {
            callback(new Error('Not allowed by CORS'));
        }
    },
    methods: 'GET,HEAD,PUT,PATCH,POST,DELETE',
    optionsSuccessStatus: 204,
    credentials: true
};


app.use(cors(corsOptions));

app.use(express.json());

const PORT = process.env.PORT || 3003;

const jwt = require('jsonwebtoken');
const { verify } = require('crypto');

// ----------------------------------------------- Middleware SECURITY ----------------------------------------------- 

// Middleware para verificar el token
function verifyToken(req, res, next) {
    const token = req.header('Authorization');
    if (!token) {
      return res.status(401).json({ message: 'Token no proporcionado' });
    }
    
  
    try {
        // Verificar el token
        const decodedToken = jwt.verify(token, 'rogo23k');
  
        // Si el token está expirado, devuelve un error
        if (decodedToken.exp < Date.now() / 1000) {
            return res.status(401).json({ error: 'Token expirado' });
        }
  
        // Si todo está bien, continuamos con la siguiente ruta
        req.user = decodedToken;
        next();
    } catch (error) {
        return res.status(401).json({ error: 'Token inválido' });
    }
  }

// --------------------------------------------------------------------------------------------------------------- 

// ----------------------------------------------- Routes API ----------------------------------------------- 

app.post('/login', authController.login)

// ---------------------------------------------------------------------------------------
app.post('/centros', centroController.crearCentro);
app.get('/centros', centroController.centros);
app.get('/centros/:id_centro', centroController.centroID);
app.put('/centros/:id_centro', centroController.actualizarCentro);
app.delete('/centros/:id_centro/desactivar', centroController.desactivarCentro);

app.get('/centros/:id_centro/usuarios', usuariosController.usuariosCentro);
//app.get('/centros/:id_centro/pacientes', pacientesController.pacientesCentro);
// ---------------------------------------------------------------------------------------


// ---------------------------------------------------------------------------------------
app.post('/usuarios', usuariosController.crearUsuario);
app.get('/usuarios', usuariosController.usuarios);
app.get('/usuarios/:id_usuario', usuariosController.usuarioID);
app.put('/usuarios/:id_usuario', usuariosController.actualizarUsuario);
app.delete('/usuarios/:id_usuario', usuariosController.desactivarUsuario);

app.get('/usuarios/:id_usuario/pacientes', usuariosController.pacientesUsuario);
// ---------------------------------------------------------------------------------------


// ---------------------------------------------------------------------------------------
app.post('/pacientes', pacientesController.crearPaciente);
app.get('/pacientes', pacientesController.pacientes);
app.get('/pacientes/:id_paciente', pacientesController.pacienteID);
app.put('/pacientes/:id_paciente', pacientesController.actualizarPaciente);
app.delete('/pacientes/:id_paciente', pacientesController.desactivarPaciente);

app.get('/pacientes/:id_paciente/usuarios', pacientesController.usuariosPaciente);
app.get('/pacientes/:id_paciente/medidas', pacientesController.medidasPaciente);
//app.get('/pacientes/:id_paciente/sensores', sensoresController.usuariosPaciente);
// ---------------------------------------------------------------------------------------


// ---------------------------------------------------------------------------------------
app.post('/sensores', sensoresController.crearSensor);
app.get('/sensores', sensoresController.sensores);
app.get('/sensores/:mac', sensoresController.sensorID);
app.put('/sensores/:mac', sensoresController.actualizarSensor);
app.put('/sensores/:mac/visible', sensoresController.desactivarSensor);

app.post('/sensores/:mac/medida', sensoresController.crearMedida)
app.get('/sensores/:mac/medidas', sensoresController.medidasSensor);
// ---------------------------------------------------------------------------------------

app.get('/', (req, res) => {
    res.send('¡Backend funcionando!');
});

app.listen(PORT, () => {
    console.log(`Servidor en ejecución en el puerto ${PORT}`);
});
