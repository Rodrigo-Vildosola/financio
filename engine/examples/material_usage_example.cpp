// #include "eng/renderer/material_manager.h"
// #include "eng/renderer/material.h"
// #include "eng/renderer/material_instance.h"
// #include "eng/renderer/pipeline.h"
// #include "eng/renderer/shader.h"
// #include "eng/core/logger.h"

// namespace eng {

// // Example demonstrating the new material system
// void material_system_example(Context& context) {
//     // Initialize the material manager
//     MaterialManager::init(context);
    
//     // Create a shader
//     ref<Shader> shader = create_ref<Shader>(
//         Shader::from_file(context, "shaders/example.wgsl", "Example Shader")
//     );
//     shader->vertex_entry = "vs_main";
//     shader->fragment_entry = "fs_main";
    
//     // Example 1: Basic material with time uniform
//     auto basic_material = MaterialManager::create_basic_material("BasicMaterial", shader);
    
//     // Example 2: Phong lighting material
//     auto phong_material = MaterialManager::create_phong_material("PhongMaterial", shader);
    
//     // Example 3: PBR material
//     auto pbr_material = MaterialManager::create_pbr_material("PBRMaterial", shader);
    
//     // Example 4: Custom material with specific parameters
//     auto custom_material = MaterialManager::create_material("CustomMaterial");
//     custom_material->set_shader(shader);
    
//     // Define custom parameters
//     custom_material->define_parameter("u_color", 0, MaterialParamType::Float3, WGPUShaderStage_Fragment);
//     custom_material->define_parameter("u_intensity", 1, MaterialParamType::Float, WGPUShaderStage_Fragment);
//     custom_material->define_parameter("u_scale", 2, MaterialParamType::Float2, WGPUShaderStage_Vertex);
    
//     // Set default values
//     f32 default_color[3] = {1.0f, 0.5f, 0.2f};
//     f32 default_scale[2] = {1.0f, 1.0f};
    
//     custom_material->set_default_float3("u_color", default_color);
//     custom_material->set_default_float("u_intensity", 1.0f);
//     custom_material->set_default_value("u_scale", default_scale, 2 * sizeof(f32));
    
//     // Create pipeline specification (simplified)
//     PipelineSpecification spec;
//     spec.shader = shader;
//     spec.surface_format = context.get_preferred_format();
    
//     // Add uniforms to pipeline spec
//     UniformBufferSpec color_uniform;
//     color_uniform.binding = 0;
//     color_uniform.size = 3 * sizeof(f32);
//     color_uniform.visibility = WGPUShaderStage_Fragment;
//     spec.uniforms.push_back(color_uniform);
    
//     UniformBufferSpec intensity_uniform;
//     intensity_uniform.binding = 1;
//     intensity_uniform.size = sizeof(f32);
//     intensity_uniform.visibility = WGPUShaderStage_Fragment;
//     spec.uniforms.push_back(intensity_uniform);
    
//     UniformBufferSpec scale_uniform;
//     scale_uniform.binding = 2;
//     scale_uniform.size = 2 * sizeof(f32);
//     scale_uniform.visibility = WGPUShaderStage_Vertex;
//     spec.uniforms.push_back(scale_uniform);
    
//     // Create pipeline
//     auto pipeline = create_scope<Pipeline>(context, spec);
    
//     // Create material instances
//     auto basic_instance = basic_material->create_instance(pipeline.get());
//     auto phong_instance = phong_material->create_instance(pipeline.get());
//     auto pbr_instance = pbr_material->create_instance(pipeline.get());
//     auto custom_instance = custom_material->create_instance(pipeline.get());
    
//     // Example usage: Update material parameters
//     f32 time = 1.5f;
//     basic_instance->set_parameter_float("u_time", time);
    
//     f32 new_color[3] = {0.8f, 0.2f, 0.9f};
//     custom_instance->set_parameter_float3("u_color", new_color);
//     custom_instance->set_parameter_float("u_intensity", 2.5f);
    
//     // Example: Using direct binding setters
//     f32 scale_values[2] = {2.0f, 1.5f};
//     custom_instance->set_float2(2, scale_values);
    
//     // Example: Using named parameters
//     custom_instance->set_parameter_float("u_intensity", 3.0f);
    
//     FNC_CORE_INFO("Material system example completed successfully!");
    
//     // Cleanup
//     MaterialManager::shutdown();
// }

// // Example showing how to create a material with multiple uniform types
// void advanced_material_example(Context& context) {
//     MaterialManager::init(context);
    
//     ref<Shader> shader = create_ref<Shader>(
//         Shader::from_file(context, "shaders/advanced.wgsl", "Advanced Shader")
//     );
    
//     auto advanced_material = MaterialManager::create_material("AdvancedMaterial");
//     advanced_material->set_shader(shader);
    
//     // Define various parameter types
//     advanced_material->define_parameter("u_model_matrix", 0, MaterialParamType::Matrix4x4, WGPUShaderStage_Vertex);
//     advanced_material->define_parameter("u_view_matrix", 1, MaterialParamType::Matrix4x4, WGPUShaderStage_Vertex);
//     advanced_material->define_parameter("u_projection_matrix", 2, MaterialParamType::Matrix4x4, WGPUShaderStage_Vertex);
//     advanced_material->define_parameter("u_light_position", 3, MaterialParamType::Float3, WGPUShaderStage_Fragment);
//     advanced_material->define_parameter("u_light_color", 4, MaterialParamType::Float3, WGPUShaderStage_Fragment);
//     advanced_material->define_parameter("u_light_intensity", 5, MaterialParamType::Float, WGPUShaderStage_Fragment);
//     advanced_material->define_parameter("u_use_texture", 6, MaterialParamType::Int, WGPUShaderStage_Fragment);
//     advanced_material->define_parameter("u_uv_offset", 7, MaterialParamType::Float2, WGPUShaderStage_Vertex);
    
//     // Set default values
//     f32 identity_matrix[16] = {
//         1.0f, 0.0f, 0.0f, 0.0f,
//         0.0f, 1.0f, 0.0f, 0.0f,
//         0.0f, 0.0f, 1.0f, 0.0f,
//         0.0f, 0.0f, 0.0f, 1.0f
//     };
    
//     f32 light_pos[3] = {0.0f, 10.0f, 0.0f};
//     f32 light_color[3] = {1.0f, 1.0f, 1.0f};
//     f32 uv_offset[2] = {0.0f, 0.0f};
    
//     advanced_material->set_default_matrix4x4("u_model_matrix", identity_matrix);
//     advanced_material->set_default_matrix4x4("u_view_matrix", identity_matrix);
//     advanced_material->set_default_matrix4x4("u_projection_matrix", identity_matrix);
//     advanced_material->set_default_float3("u_light_position", light_pos);
//     advanced_material->set_default_float3("u_light_color", light_color);
//     advanced_material->set_default_float("u_light_intensity", 1.0f);
//     advanced_material->set_default_value("u_use_texture", (void*) {0}, sizeof(i32));
//     advanced_material->set_default_value("u_uv_offset", uv_offset, 2 * sizeof(f32));
    
//     FNC_CORE_INFO("Advanced material created with multiple parameter types!");
    
//     MaterialManager::shutdown();
// }

// } // namespace eng 
