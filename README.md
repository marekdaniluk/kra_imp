# kra_imp

**Krita KRA File Import Library (C++)**

## 📖 About

**kra_imp** is a high-performance C++ library for reading **KRA files**, which are archives used by the [Krita digital painting application](https://krita.org/). This library provides an API for opening and reading KRA files, enabling seamless integration of KRA support into third-party applications.

### ✨ Features

- **🚀 High Efficiency** – Designed for zero memory allocations.
- **🔗 Easy Integration** – C-style API for compatibility with various projects.
- **🔒 Stability** – Fully covered with unit tests.
- **🎞️ Animation Support** – Reads both image layers and animation keyframes.

---

## 🛠 Build Instructions

This project uses [CMake](https://cmake.org/) as its build system. Follow these steps to compile the library:

### 1️⃣ Clone the Repository

```sh
git clone --recursive https://github.com/marekdaniluk/kra_imp.git
```

The **kra_imp** library relies on the following dependencies, which are included as submodules:

- [pugixml](https://github.com/zeux/pugixml) – Fast XML parser.
- [zip](https://github.com/kuba--/zip) – Lightweight ZIP archive handling.
- [Catch2](https://github.com/catchorg/Catch2) – Unit testing framework (used only for tests).

### 2️⃣ Create a Build Directory

```sh
cmake -S . -B build
```

### 3️⃣ Build the Library

```sh
cmake --build build
```

---

## 📖 Documentation

### 📝 API Documentation

The API documentation can be generated using [Doxygen](https://www.doxygen.nl/) from the source code.

### 📄 KRA File Format Overview

KRA is Krita's native file format, designed to encapsulate all elements of a digital artwork project while preserving its structure and editability. A KRA file is essentially a ZIP archive. By renaming a `.kra` file to `.zip`, you can extract its contents with any standard decompression tool.

#### **Key Files & Directories in a KRA Archive:**

- **`maindoc.xml`** – Defines the document structure, including layer hierarchy and properties.
- **`mergedimage.png`** – A flattened representation of the artwork.
- **`preview.png`** – A small thumbnail preview of the artwork.
- **`documentinfo.xml`** – Stores metadata, such as editing history and document-specific details.
- **`mimetype`** – Identifies the document as a Krita file (`application/x-krita`).
- **`layers/`** – Contains individual layer data:
  - `layer1`, `layer2`, etc. – Store pixel data and layer properties.
  - `layer1.defaultpixel`, `layer2.defaultpixel` – Unknown function.
  - `layer1.icc`, `layer2.icc` – ICC color profiles, mainly used for printing.
- **`annotations/`** – Contains files of unknown purpose.
- **`animation/`** – Contains files related to Krita’s animation features (specific purpose still undocumented).

---

## 🔍 Code Example

```cpp
#include <kra_imp/kra_imp.hpp>
#include <format>
#include <fstream>
#include <vector>

int main() {
    // Load KRA archive into a binary buffer
    std::vector<char> archive_file_buffer;
    std::ifstream fin("example.kra", std::ios::binary);
    fin.seekg(0, std::ios_base::end);
    auto length = fin.tellg();
    archive_file_buffer.resize(length);
    fin.seekg(0, std::ios_base::beg);
    fin.read(archive_file_buffer.data(), length);
    fin.close();

    // Open the archive
    kra_imp_archive_t* archive = kra_imp_open_archive(archive_file_buffer.data(), archive_file_buffer.size());
    
    // Load maindoc.xml
    const unsigned long long maindoc_file_size = kra_imp_get_file_size(archive, kra_imp_get_main_doc_name());
    std::vector<char> maindoc_file_buffer(maindoc_file_size);
    kra_imp_load_file(archive, kra_imp_get_main_doc_name(), maindoc_file_buffer.data(), maindoc_file_size);

    // Read maindoc.xml
    kra_imp_main_doc_t main_doc;
    kra_imp_read_main_doc(maindoc_file_buffer.data(), maindoc_file_size, &main_doc);

    // Read image layers
    kra_imp_image_layer_t image_layer;
    for (std::size_t i = 0; i < main_doc._layers_count; ++i) {
        kra_imp_read_image_layer(maindoc_file_buffer.data(), maindoc_file_size, i, &image_layer);
        if (image_layer._type != KRA_IMP_PAINT_LAYER_TYPE) {
          continue;
        }

        // Load image layer file
        const std::string layer_file_path = std::format("{}/{}/{}", main_doc._image_name, kra_imp_get_layer_directory_name(), image_layer._file_name);
        const unsigned long long layer_file_size = kra_imp_get_file_size(archive, layer_file_path.data());
        std::vector<char> layer_file_buffer(layer_file_size);
        kra_imp_load_file(archive, layer_file_path.data(), layer_file_buffer.data(), layer_file_size);
    }

    // Close the archive
    kra_imp_close_archive(archive);

    return 0;
}
```

---

## ⚠️ Limitations

- The library has only been tested in a Windows environment.
- The `CMakeLists.txt` script is minimal and may require adjustments to integrate into a project.
- Only **RGB/Alpha** color space with **8-bit integer per channel** is supported.

---

## 📜 License

This project is licensed under the **MIT License**. See the [LICENSE](LICENSE) file for details.

---

## 🤝 Contributing

Contributions are welcome! If you’d like to improve **kra_imp**, please follow these steps:

1. **Fork** the repository.
2. Create a new branch (`git checkout -b feature-name`).
3. Commit your changes (`git commit -m "Add new feature"`).
4. Push to your branch (`git push origin feature-name`).
5. Submit a **Pull Request**.

---

## 📬 Contact

For questions, issues, or suggestions, feel free to open an issue on the repository.
