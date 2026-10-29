from setuptools import setup


package_name = "tempo_ros_lab"


setup(
    name=package_name,
    version="0.1.0",
    packages=[package_name],
    data_files=[
        ("share/ament_index/resource_index/packages", ["resource/" + package_name]),
        ("share/" + package_name, ["package.xml"]),
    ],
    install_requires=["setuptools"],
    zip_safe=True,
    maintainer="Tempo Experimentos",
    maintainer_email="unreal@todo.todo",
    description="Incremental ROS 2 Python experiments for the Tempo Unreal project.",
    license="MIT",
    entry_points={
        "console_scripts": [
            "print_hello_world = tempo_ros_lab.print_hello_world:main",
            "move_and_rotate = tempo_ros_lab.move_and_rotate:main",
        ],
    },
)
