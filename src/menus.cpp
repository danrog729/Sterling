#include "menus.h"
#include "maths.h"
#include "primitives.h"

namespace menus
{
	Object* selectedObject = NULL;
	Camera* selectedCamera = NULL;
	Light* selectedLight = NULL;
	AmbientLight* selectedAmbientLight = NULL;
	PointLight* selectedPointLight = NULL;
	Spotlight* selectedSpotlight = NULL;
	DirectionalLight* selectedDirectionalLight = NULL;
	bool wireframe = false;

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
		ImGui::Begin("Tree View");
		if (ImGui::TreeNodeEx("Scene", ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (ImGui::IsItemClicked())
			{
				selectedObject = NULL;
				selectedCamera = NULL;
				selectedLight = NULL;
				selectedAmbientLight = NULL;
				selectedPointLight = NULL;
				selectedSpotlight = NULL;
				selectedDirectionalLight = NULL;
			}
			if (ImGui::BeginDragDropTarget())
			{
				// place down an object
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("STERLING_OBJECT"))
				{
					IM_ASSERT(payload->DataSize == sizeof(Object*));
					Object* toAdd = *(Object**)(payload->Data);
					toAdd->remove_from_parent();
					toAdd->parent = NULL;
					scene->children.push_back(toAdd);
				}
				ImGui::EndDragDropTarget();
			}
			unsigned int id = 0;
			for (int childIndex = 0; childIndex < scene->children.size(); childIndex++)
			{
				menus::object_tree(scene->children[childIndex], &id);
			}
			ImGui::TreePop();
		}
		if (ImGui::Button("Add Object"))
		{
			ImGui::OpenPopup("Add Object");
		}
		if (ImGui::BeginPopup("Add Object"))
		{
			if (ImGui::Button("Add Cube"))
			{
				ImGui::CloseCurrentPopup();
				scene->add_object(primitives::cube(scene, "cube"));
			}
			if (ImGui::Button("Add Plane"))
			{
				ImGui::CloseCurrentPopup();
				scene->add_object(primitives::plane(scene, "plane"));
			}
			if (ImGui::Button("Add Sphere"))
			{
				ImGui::CloseCurrentPopup();
				scene->add_object(primitives::sphere(scene, "sphere", 32, 17));
			}
			if (ImGui::Button("Add Ambient Light"))
			{
				ImGui::CloseCurrentPopup();
				scene->add_object(new AmbientLight(scene, "ambient light"));
			}
			if (ImGui::Button("Add Point Light"))
			{
				ImGui::CloseCurrentPopup();
				scene->add_object(new PointLight(scene, "point light"));
			}
			if (ImGui::Button("Add Spotlight"))
			{
				ImGui::CloseCurrentPopup();
				scene->add_object(new Spotlight(scene, "spotlight"));
			}
			if (ImGui::Button("Add Directional Light"))
			{
				ImGui::CloseCurrentPopup();
				scene->add_object(new DirectionalLight(scene, "directional light"));
			}
			ImGui::EndPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Delete Object"))
		{
			if (selectedObject != NULL)
			{
				delete selectedObject;
				selectedObject = NULL;
				selectedCamera = NULL;
				selectedLight = NULL;
				selectedAmbientLight = NULL;
				selectedPointLight = NULL;
				selectedSpotlight = NULL;
				selectedDirectionalLight = NULL;
			}
		}
		ImGui::End();
	}

	void menus::object_tree(Object* object, unsigned int* ID)
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
		std::string string = object->objectName;
		string += "##";
		string += std::to_string(*ID);
		(*ID)++;
		if (ImGui::TreeNodeEx(string.c_str(), flags))
		{
			if (ImGui::IsItemClicked())
			{
				selectedObject = object;
				selectedCamera = dynamic_cast<Camera*>(selectedObject);
				selectedLight = dynamic_cast<Light*>(selectedObject);
				selectedAmbientLight = dynamic_cast<AmbientLight*>(selectedObject);
				selectedPointLight = dynamic_cast<PointLight*>(selectedObject);
				selectedSpotlight = dynamic_cast<Spotlight*>(selectedObject);
				selectedDirectionalLight = dynamic_cast<DirectionalLight*>(selectedObject);
			}
			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
			{
				// pick up an object
				ImGui::SetDragDropPayload("STERLING_OBJECT", &object, sizeof(Object*));
				ImGui::Text(object->objectName);
				ImGui::EndDragDropSource();
			}
			if (ImGui::BeginDragDropTarget())
			{
				// place down an object
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("STERLING_OBJECT"))
				{
					IM_ASSERT(payload->DataSize == sizeof(Object*));
					Object* toAdd = *(Object**)(payload->Data);
					toAdd->remove_from_parent();
					toAdd->parent = object;
					object->children.push_back(toAdd);
				}
				ImGui::EndDragDropTarget();
			}
			for (int childIndex = 0; childIndex < object->children.size(); childIndex++)
			{
				menus::object_tree(object->children[childIndex], ID);
			}
			ImGui::TreePop();
		}
	}

	void menus::properties()
	{
		ImGui::Begin("Properties");
		if (selectedObject != NULL)
		{
			bool hasPosition = selectedAmbientLight == NULL && selectedDirectionalLight == NULL;
			bool hasRotation = selectedAmbientLight == NULL && selectedPointLight == NULL;
			bool hasScale = selectedCamera == NULL && selectedAmbientLight == NULL && selectedPointLight == NULL && selectedSpotlight == NULL && selectedDirectionalLight == NULL;
			bool hasColour = selectedLight != NULL;
			bool hasAttenuation = selectedPointLight != NULL || selectedSpotlight != NULL;
			bool hasCutoff = selectedSpotlight != NULL;

			ImGui::Text(selectedObject->objectName);
			if (hasPosition || hasRotation || hasScale)
			{
				if (ImGui::TreeNodeEx("Transformation", ImGuiTreeNodeFlags_DefaultOpen))
				{
					if (hasPosition)
					{
						ImGui::Text("Position");
						ImGui::SameLine();
						maths::vec3f position = selectedObject->transformation.position();
						bool changePos = false;
						if (ImGui::Button("Reset##Position"))
						{
							position = maths::vec3f(0.0f, 0.0f, 0.0f);
							changePos = true;
						}
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
					}
					if (hasRotation)
					{
						ImGui::Text("Rotation");
						ImGui::SameLine();
						maths::vec3f rotation = selectedObject->transformation.rotation().to_euler_angles();
						rotation = rotation * 180.0f / maths::PI;
						bool changeRot = false;
						if (ImGui::Button("Reset##Rotation"))
						{
							rotation = maths::vec3f(0.0f, 0.0f, 0.0f);
							changeRot = true;
						}
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
					}
					if (hasScale)
					{
						ImGui::Text("Scale");
						ImGui::SameLine();
						maths::vec3f scale = selectedObject->transformation.scale();
						bool changeScale = false;
						if (ImGui::Button("Reset##Scale"))
						{
							scale = maths::vec3f(1.0f, 1.0f, 1.0f);
							changeScale = true;
						}
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
					}
					ImGui::TreePop();
				}
			}
			if (hasColour)
			{
				ImGui::Text("Colour");
				ImGui::SameLine();
				maths::vec3f colour = selectedLight->colour();
				bool changeCol = false;
				if (ImGui::Button("Reset##Colour"))
				{
					colour = maths::vec3f(1.0f, 1.0f, 1.0f);
					changeCol = true;
				}
				if (ImGui::DragFloat("Red", &colour.x, 0.001f))
				{
					changeCol = true;
				}
				if (ImGui::DragFloat("Green", &colour.y, 0.001f))
				{
					changeCol = true;
				}
				if (ImGui::DragFloat("Blue", &colour.z, 0.001f))
				{
					changeCol = true;
				}
				if (changeCol)
				{
					selectedLight->colour(colour);
				}
			}
			if (hasAttenuation)
			{
				ImGui::Text("Attenuation");
				ImGui::SameLine();
				float constant;
				float linear;
				float quadratic;
				if (selectedPointLight != NULL)
				{
					constant = selectedPointLight->constantAttenuation();
					linear = selectedPointLight->linearAttenuation();
					quadratic = selectedPointLight->quadraticAttenuation();
				}
				else if (selectedSpotlight != NULL)
				{
					constant = selectedSpotlight->constantAttenuation();
					linear = selectedSpotlight->linearAttenuation();
					quadratic = selectedSpotlight->quadraticAttenuation();
				}
				bool changeAttenuation = false;
				if (ImGui::Button("Reset##Attenuation"))
				{
					constant = 1.0f;
					linear = 0.07f;
					quadratic = 0.017f;
					changeAttenuation = true;
				}
				if (ImGui::DragFloat("Constant", &constant, 0.01f))
				{
					changeAttenuation = true;
				}
				if (ImGui::DragFloat("Linear", &linear, 0.0001f))
				{
					changeAttenuation = true;
				}
				if (ImGui::DragFloat("Quadratic", &quadratic, 0.00001f))
				{
					changeAttenuation = true;
				}
				if (changeAttenuation && selectedPointLight != NULL)
				{
					selectedPointLight->constantAttenuation(constant);
					selectedPointLight->linearAttenuation(linear);
					selectedPointLight->quadraticAttenuation(quadratic);
				}
				else if (changeAttenuation && selectedSpotlight != NULL)
				{
					selectedSpotlight->constantAttenuation(constant);
					selectedSpotlight->linearAttenuation(linear);
					selectedSpotlight->quadraticAttenuation(quadratic);
				}
			}
			if (hasCutoff)
			{
				ImGui::Text("Cutoff");
				ImGui::SameLine();
				float inner = selectedSpotlight->innerCutoff();
				float outer = selectedSpotlight->outerCutoff();
				bool changeCutoff = false;
				if (ImGui::Button("Reset##Cutoff"))
				{
					inner = 0.45f;
					outer = maths::PI / 6;
					changeCutoff = true;
				}
				inner *= 180.0f / maths::PI;
				outer *= 180.0f / maths::PI;
				if (ImGui::DragFloat("Inner", &inner, 0.01f))
				{
					changeCutoff = true;
				}
				if (ImGui::DragFloat("Outer", &outer, 0.01f))
				{
					changeCutoff = true;
				}
				inner *= maths::PI / 180.0f;
				outer *= maths::PI / 180.0f;
				if (changeCutoff)
				{
					selectedSpotlight->innerCutoff(inner);
					selectedSpotlight->outerCutoff(outer);
				}
			}
		}
		else
		{
			ImGui::Text("No object selected.");
		}
		ImGui::End();
	}

	void menus::settings()
	{
		if (ImGui::Begin("Settings"))
		{
			ImGui::Checkbox("Wireframe", &wireframe);
		}
		ImGui::End();
	}
}