# Quick and dirty script to set up tools in Unreal Engine's editor menu.
# This script creates a custom "Animation Pipeline Tools" menu with a submenu for launching an "Anim Exporter" tool.
# It is intended for rapid setup and prototyping purposes, not for production-ready use.

import unreal
from typing import Optional

PIPELINE_TOOL = "AnimationPipelineTool"
PIPELINE_TOOL_MENU = "AnimationPipelineToolsMenu"
PIPELINE_SECTION = "AnimationPipelineSection"

def create_tools_menu():
    create_main_menu()

    section = "Window"
    name = "Anim Exporter"
    label = "Anim Exporter"
    tool_tip = "Anim Exporter"
    widget_path = '/AnimExporter/UI/EUW_AnimExporter.EUW_AnimExporter'
    command = f'launch_widget("{widget_path}")'

    add_anim_exporter_menu(section, name, label, tool_tip, command)

def create_main_menu() -> unreal.ToolMenu:
    owner = unreal.Name(PIPELINE_TOOL)
    parent_name = unreal.Name("LevelEditor.MainMenu")
    section_name = unreal.Name("Window")
    name = unreal.Name(PIPELINE_TOOL_MENU)
    label = unreal.Text("Animation Pipeline Tools")

    menus = unreal.ToolMenus.get()
    parent_menu = menus.find_menu(parent_name)
    if not parent_menu:
        unreal.log_error(f"Failed to find menu: {parent_name}")

    menu = parent_menu.add_sub_menu(owner, section_name, name, label)
    menus.refresh_all_widgets()
    return menu

def get_tools_menu() -> unreal.ToolMenu:
    menus = unreal.ToolMenus.get()
    pipeline_tools_menu = menus.find_menu(unreal.Name(f"LevelEditor.MainMenu.{PIPELINE_TOOL_MENU}"))
    if not pipeline_tools_menu:
        unreal.log_error(f"Failed to find menu: LevelEditor.MainMenu.{PIPELINE_TOOL_MENU}")
    return pipeline_tools_menu

def add_anim_exporter_menu(section, name, label, tool_tip, command):
    menu = get_tools_menu()
    tool_menu_entry_name = unreal.Name(name)
    py_script_entry = unreal.ToolMenuEntry(
        name=tool_menu_entry_name,
        type=unreal.MultiBlockType.MENU_ENTRY
    )

    label_text = unreal.Text(label)
    tool_tip_text = unreal.Text(tool_tip)
    py_script_entry.set_label(label_text)

    py_script_entry.set_string_command(unreal.ToolMenuStringCommandType.PYTHON, unreal.Name(""), str(command))
    py_script_entry.set_tool_tip(tool_tip_text)
    menu.add_menu_entry(unreal.Name(section), py_script_entry)


def launch_widget(widget_path)-> Optional[unreal.EditorUtilityWidget]:
    editor_subsystem = unreal.get_editor_subsystem(unreal.EditorUtilitySubsystem)
    # Spawn and register the widget tab
    widget_class = unreal.load_object(None, widget_path)
    widget = editor_subsystem.spawn_and_register_tab(widget_class)
    return widget

create_tools_menu()