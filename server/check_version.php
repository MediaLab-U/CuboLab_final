<?php
// Obtener el parámetro 'v' de la URL
$v = isset($_GET['v']) ? $_GET['v'] : null;

// Comprobar si el parámetro 'v' está definido y es un número válido
if ($v !== null && is_numeric($v)) {
    // Convertir 'v' a un número flotante
    $v = floatval($v);

    // Comprobar el valor de 'v' y establecer el código de estado HTTP correspondiente
    if ($v < 1.1) {
        // Responder con 200 OK si v < 1.1
        http_response_code(200);
    } else {
        // Responder con 304 Not Modified si v >= 1.0
        http_response_code(500);
    }
} else {
    // Responder con 400 Bad Request si el parámetro 'v' no está definido o no es válido

    http_response_code(400);
    echo "400 Bad Request - Parámetro 'v' no definido o no es válido";
}
?>
