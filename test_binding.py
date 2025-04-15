from genericpath import isfile
import os
import time
import threading
import psutil

import image_loader


class ThreadMonitor:
    def __init__(self):
        self.process = psutil.Process()
        self.max_threads = 0
        self.running = False

    def __enter__(self):
        self.running = True
        self.monitor_thread = threading.Thread(target=self._track_threads)
        self.monitor_thread.start()
        return self

    def __exit__(self, *args):
        self.running = False
        self.monitor_thread.join()

    def _track_threads(self):
        while self.running:
            current = self.process.num_threads()
            if current > self.max_threads:
                self.max_threads = current
            time.sleep(0.001)


def test_thread_usage():
    textures_dir = "Textures"
    supported_input_ext = {"image4ub", "image4f"}  # Только входные форматы
    files = [
        f
        for f in os.listdir(textures_dir)
        if isfile(os.path.join(textures_dir, f)) and f.split(".")[-1].lower() in supported_input_ext
    ]

    print(f"System CPU cores: {os.cpu_count()}")
    print(f"Base threads: {psutil.Process().num_threads()}")

    for texture_name in files:
        texture_path = os.path.join(textures_dir, texture_name)
        file_format = get_format_from_extension(texture_name)

        if file_format == "unknown":
            print(f"Skipping unsupported file: {texture_name}")
            continue

        print(f"\nProcessing: {texture_path}")

        try:
            with ThreadMonitor() as monitor:
                # Загрузка метаданных
                info = image_loader.get_image_info(texture_path)
                print(f"[Load Metadata] Threads: {monitor.max_threads}")
                print(
                    f"Image info: width={info.width}, height={info.height}, channels={info.channels}"
                )

                # Валидация размеров
                if info.width <= 0 or info.height <= 0 or info.channels not in {3, 4}:
                    print(f"Invalid image dimensions/channels: {texture_path}")
                    continue

                data = None
                if file_format == "image4ub":
                    data = image_loader.loadImage4ub(info.path)
                elif file_format == "image4f":
                    data = image_loader.loadImage4f(info.path, info.channels)

                print(f"[Load Data] Threads: {monitor.max_threads}")

                # Сохранение
                output_name = os.path.splitext(texture_name)[0] + "png"
                output_path = os.path.join("python_converted", output_name)  # Отдельная папка

                os.makedirs("python_converted", exist_ok=True)

                image_loader.save_image_ldr(
                    output_path, data, info.width, info.height, info.channels
                )

                print(f"[Save PNG] Threads: {monitor.max_threads}")
                print(f"Saved: {output_path}")
                print(f"Size: {os.path.getsize(output_path)} bytes")

        except Exception as e:
            print(f"Fatal error: {str(e)}")


def get_format_from_extension(filename):
    """Определяет формат изображения по расширению файла."""
    ext = filename.split(".")[-1].lower()
    return {"image4ub": "image4ub", "image4f": "image4f"}.get(ext, "unknown")


if __name__ == "__main__":
    test_thread_usage()
