# derApkBuilder

A Android Package Builder CLI

---

## Features

- [x] APK Compilation
- [ ] AAB Support
- [x] Java
- [ ] Kotlin
- [ ] R8 / ProGuard
- [ ] ViewBinding
- [ ] Jetpack Compose (not supported yet)

---

## Project Structure

Your Android project must follow this structure:

```text
Project/
 ├── src/
 │    ├── java/          (Java source files)
 │    └── res/           (resources)
 │
 ├── AndroidManifest.xml
 ├── keystore.jks (to sign apk)
 └── project.yml (project config)


---

Requirements (Linux 🐧)

• javac
• aapt2
• d8
• Android SDK


---

Building the CLI

chmod +x make.sh
./make.sh


---

Building an APK

./derapkbuilder run <PROJECT_PATH>

Example

./derapkbuilder run example/


---

Configuration

All configurations are defined in project.yml (see example in the repository).