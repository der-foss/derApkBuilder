Configuration
=============

All project settings are defined in the ``project.yml`` file.

Below is a detailed explanation of each available configuration option.

---

Root Options
------------

.. code-block:: yaml

   name: my-project
   build-path: .build

**name**  
    The name of the project. This is used to identify the build and will typically be used as the final APK file name.

**build-path**  
    Directory where all build artifacts will be generated, including intermediate files and the final APK.

---

Binaries (Optional)
------------------

.. code-block:: yaml

   bins:
     aapt2: bin/aapt2
     javac: bin/javac
     jar: bin/jar
     jarsigner: bin/jarsigner
     d8: bin/d8
     zip: bin/zip
     zipalign: bin/zipalign

This section allows you to override the default binaries used during the build process.

Each field defines the path to a specific tool:

**aapt2**  
    Android Asset Packaging Tool v2. Used to compile and package Android resources.

**javac**  
    Java compiler used to compile ``.java`` files into ``.class`` files.

**jar**  
    Packages compiled classes into ``.jar`` files.

**jarsigner**  
    Signs the APK using a keystore.

**d8**  
    Converts Java bytecode (``.class``) into Dalvik bytecode (``.dex``).

**zip**  
    Packages files into a ZIP archive (APK format).

**zipalign**  
    Optimizes APK alignment for better performance on Android devices.

**Note:**  
    All fields in this section are optional. If not provided, the CLI will attempt to use the binaries available in the system ``PATH``.

---

Android
-------

.. code-block:: yaml

   android:
     sdk-api-version: 29
     sdk-min-api-version: 24

     sdk-path: env(ANDROID_SDK)
     manifest-path: AndroidManifest.xml
     res-path: src/res
     java-path: src/java

     keystore-path: myproject.keystore
     keystore-alias: myproject
     keystore-store-pass: 87654321
     keystore-key-pass: 87654321

**sdk-api-version**  
    Target Android API level used to compile the application.

**sdk-min-api-version**  
    Minimum Android API level required to run the application.

**sdk-path** *(optional)*  
    Path to the Android SDK. You can also use environment variables like ``env(ANDROID_SDK)``.

**manifest-path**  
    Path to the ``AndroidManifest.xml`` file.

**res-path**  
    Path to the Android resources directory (``res``).

**java-path**  
    Path to the Java source code directory.

**keystore-path**  
    Path to the keystore file used to sign the APK.

**keystore-alias**  
    Alias name of the key inside the keystore.

**keystore-store-pass**  
    Password for the keystore.

**keystore-key-pass**  
    Password for the specific key.

---

Dependencies
------------

.. code-block:: yaml

   dependencies:
     - androidx.constraintlayout:constraintlayout:2.2.0
     - md3.hehe:mrje:2.2.0
     - java.ejhej:mrje:8.0.2

Defines external dependencies required by the project.

Each dependency follows the format:

::

   group:artifact:version

- **group**  
    The package namespace or organization.

- **artifact**  
    The library name.

- **version**  
    The specific version of the dependency.

These dependencies will be resolved and included during the build process.