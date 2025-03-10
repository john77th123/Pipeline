# Unreal Engine Animation Exporter Plugin

This project showcases the **Animation Exporter Plugin** and its use case within Unreal Engine. While Unreal Engine provides many tools to set up an in-editor animation pipeline, it lacks built-in features to effectively manage and export animation data. Animation exporters have long been a crucial part of the animation content integration process, and this plugin aims to fill that gap in Unreal Engine Editor based animation pipeline.

---

## Overview

The plugin is inspired by the behavior of Maya's animation exporter, ensuring that animation sets remain consistent with their corresponding animation clips. Each animation clip contains essential metadata such as:

- **Name**
- **Start frame**
- **End frame**
- **Animation layers**

Replicating Maya's animation layer behavior within the Unreal Engine editor presents a unique challenge. This plugin leverages Unreal's **Level Sequence Track System** as the foundation for replicating Maya's scene structure and implementing animation layers. Additive and subtractive tracks are used to mimic animation layer behavior effectively. While the tool's functionality is focusing on managing and exporting animation sequence, it can be easily extended to manage additive blendtype, reference pose of exported animation sequence asset to reduce asset management overhead and possible bugs.

---

## Demo

https://github.com/user-attachments/assets/7fa8cc13-9d5a-499d-8870-d17b38c93514

---

## Video Overview

[https://github.com/user-attachments/assets/7fa8cc13-9d5a-499d-8870-d17b38c93514](https://drive.google.com/file/d/1T-44TAuTJ1bjJRFHyj4i4UZ4ZKXYicaY/view?usp=sharing)

---


## Features

- **Animation Layer Support**: Implements animation layers using Unreal Engine's additive and subtractive tracks.
- **Seamless Integration**: Leverages the Level Sequence Track System to mirror the Maya scene.
- **Enhanced UI**: Uses Unreal's **Editor Utility Widget** for the user interface, ensuring ease of use and future expandability.
- **Sequencer Integration**: Utilizes available ViewModel classes to update the Sequencer track UI, despite limitations in Unreal's exposed API for UI-related classes.

---

## Limitations

This tool is a **work in progress (WIP)** and may have limitations or bugs as it was intended for learning purposes.  
**Please use it at your own risk.**

---

## Installation

1. Clone or download this repository.
2. Add the plugin folder to your Unreal Engine project's `Plugins` directory.
3. Enable the plugin in the Unreal Engine editor.
4. Restart the editor to apply changes.

---

## Contribution

Huge thanks to Melyssa Battison for help with navigating Unreal Engine c++ basics and various functions, Casey McDermott for pushing for the idea of UE animation pipeline.
Contributions, issues, and feature requests are welcome! Feel free to open an issue or submit a pull request.
