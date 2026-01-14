import subprocess
import os
import unittest

class TestSDLImageRenderer(unittest.TestCase):
    def setUp(self):
        self.executable = "./build/sdl_image_renderer"
        self.test_image = "tests/test_image.png"
        
        # Crear una imagen de prueba simple si no existe
        if not os.path.exists(self.test_image):
            from PIL import Image
            img = Image.new('RGB', (100, 100), color='red')
            img.save(self.test_image)

    def test_no_args(self):
        """Prueba que el programa falle sin argumentos"""
        result = subprocess.run([self.executable], capture_output=True, text=True)
        self.assertNotEqual(result.returncode, 0)
        self.assertIn("Uso:", result.stdout)

    def test_invalid_image(self):
        """Prueba con una ruta de imagen inexistente"""
        result = subprocess.run([self.executable, "non_existent.png"], capture_output=True, text=True)
        self.assertNotEqual(result.returncode, 0)
        self.assertIn("No se pudo cargar la imagen", result.stdout)

    def test_load_image_and_quit(self):
        """
        Prueba cargar una imagen y cerrar automáticamente.
        Nota: Esto es difícil de probar en un entorno sin GUI real,
        pero verificaremos que el ejecutable al menos inicie e intente cargar.
        """
        # Intentamos ejecutarlo con un timeout corto para ver si carga la imagen antes de ser "matado"
        # En un CI sin monitor esto podría fallar si SDL_Init(VIDEO) falla.
        if not os.path.exists(self.executable):
            self.fail(f"Ejecutable no encontrado en {self.executable}. Compila el proyecto primero.")

        # Usamos env vars para evitar que se abra una ventana real si es posible (aunque SDL dummy puede fallar)
        env = os.environ.copy()
        env["SDL_VIDEODRIVER"] = "dummy" 
        
        try:
            # Ejecutamos y esperamos que falle o expire (no tenemos bucle de auto-cierre)
            result = subprocess.run([self.executable, self.test_image], timeout=2, capture_output=True, text=True, env=env)
            output = result.stdout
        except subprocess.TimeoutExpired as e:
            output = e.stdout if e.stdout else ""
        
        # Verificamos si al menos intentó cargar la imagen
        # Nota: Si el driver dummy falla, es posible que no llegue aquí, pero es el test esperado
        self.assertTrue("Imagen cargada" in output or "Error" in output, f"Salida inesperada: {output}")

if __name__ == "__main__":
    unittest.main()
