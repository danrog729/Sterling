#include "menus.h"
#include "maths.h"

namespace menus
{
	Object* selectedObject = NULL;
}

void menus::setup(GLFWwindow* window)
{
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();
}

void menus::shutdown()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void menus::render()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void menus::refresh()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void menus::scene_tree(Scene* scene)
{
	ImGui::Begin("Scene");
	for (int childIndex = 0; childIndex < scene->children.size(); childIndex++)
	{
		menus::object_tree(scene->children[childIndex]);
	}
	ImGui::End();
}

void menus::object_tree(Object* object)
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
	if (object->children.size() == 0)
	{
		flags |= ImGuiTreeNodeFlags_Leaf;
	}
	if (menus::selectedObject == object)
	{
		flags |= ImGuiTreeNodeFlags_Selected;
	}
	if (ImGui::TreeNodeEx(object->objectName, flags))
	{
		if (ImGui::IsItemClicked())
		{
			menus::selectedObject = object;
		}
		for (int childIndex = 0; childIndex < object->children.size(); childIndex++)
		{
			menus::object_tree(object->children[childIndex]);
		}
		ImGui::TreePop();
	}
}

void menus::properties()
{
	ImGui::Begin("Properties");
	if (selectedObject != NULL)
	{
		ImGui::Text(selectedObject->objectName);
		if (ImGui::TreeNodeEx("Transformation"))
		{
			ImGui::Text("Position");
			maths::vec3f position = selectedObject->transformation.position();
			bool changePos = false;
			if (ImGui::DragFloat("X##Pos", &position.x, 0.01f))
			{
				changePos = true;
			}
			if (ImGui::DragFloat("Y##Pos", &position.y, 0.01f))
			{
				changePos = true;
			}
			if (ImGui::DragFloat("Z##Pos", &position.z, 0.01f))
			{
				changePos = true;
			}
			if (changePos)
			{
				selectedObject->transformation.position(position);
			}

			ImGui::Text("Rotation");
			maths::vec3f rotation = selectedObject->transformation.rotation().to_euler_angles();
			rotation = rotation * 180.0f / maths::PI;
			bool changeRot = false;
			if (ImGui::DragFloat("Yaw", &rotation.z, 0.1f))
			{
				changeRot = true;
			}
			if (ImGui::DragFloat("Pitch", &rotation.y, 0.1f))
			{
				changeRot = true;
			}
			if (ImGui::DragFloat("Roll", &rotation.x, 0.1f))
			{
				changeRot = true;
			}
			if (changeRot)
			{
				rotation = rotation * maths::PI / 180.0f;
				selectedObject->transformation.rotation(maths::unit_quaternion::from_euler_angles(rotation));
			}

			ImGui::Text("Scale");
			maths::vec3f scale = selectedObject->transformation.scale();
			bool changeScale = false;
			if (ImGui::DragFloat("X##Scale", &scale.x, 0.01f))
			{
				changeScale = true;
			}
			if (ImGui::DragFloat("Y##Scale", &scale.y, 0.01f))
			{
				changeScale = true;
			}
			if (ImGui::DragFloat("Z##Scale", &scale.z, 0.01f))
			{
				changeScale = true;
			}
			if (changeScale)
			{
				selectedObject->transformation.scale(scale);
			}

			ImGui::TreePop();
		}
	}
	else
	{
		ImGui::Text("No object selected.");
	}
	ImGui::End();
}