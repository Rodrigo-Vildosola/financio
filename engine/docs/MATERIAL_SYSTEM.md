# Material System Documentation

## Overview

The Terra engine now features a comprehensive material system that replaces hardcoded uniforms with a flexible, type-safe material instance system. This system provides better organization, reusability, and maintainability for shader parameters.

## Key Components

### 1. MaterialInstance
The core class that manages uniforms for a specific pipeline instance.

**Features:**
- Type-safe uniform setters (Float, Float2, Float3, Float4, Int, Int2, Int3, Int4, Matrix4x4)
- Named parameter support
- Automatic GPU buffer management
- Bind group creation and management

### 2. Material
A template class that defines the structure and default values for material parameters.

**Features:**
- Parameter definitions with types and bindings
- Default value management
- Material instance creation
- Shader association

### 3. MaterialManager
A centralized system for creating and managing materials.

**Features:**
- Predefined material templates (Basic, Phong, PBR)
- Material caching and reuse
- Global material management
- Easy material instance creation

## Usage Examples

### Basic Usage

```cpp
// Initialize the material manager
MaterialManager::init(context);

// Create a basic material
auto material = MaterialManager::create_basic_material("MyMaterial", shader);

// Create a pipeline
auto pipeline = create_scope<Pipeline>(context, pipeline_spec);

// Create a material instance
auto instance = material->create_instance(pipeline.get());

// Update parameters
instance->set_parameter_float("u_time", 1.5f);

// Bind during rendering
instance->bind(pass_encoder);
```

### Custom Material Creation

```cpp
// Create a custom material
auto material = MaterialManager::create_material("CustomMaterial");
material->set_shader(shader);

// Define parameters
material->define_parameter("u_color", 0, MaterialParamType::Float3, WGPUShaderStage_Fragment);
material->define_parameter("u_intensity", 1, MaterialParamType::Float, WGPUShaderStage_Fragment);

// Set default values
f32 default_color[3] = {1.0f, 0.5f, 0.2f};
material->set_default_float3("u_color", default_color);
material->set_default_float("u_intensity", 1.0f);

// Create instance
auto instance = material->create_instance(pipeline.get());
```

### Advanced Parameter Types

```cpp
// Matrix parameters
f32 model_matrix[16] = { /* identity matrix */ };
instance->set_matrix4x4("u_model", model_matrix);

// Vector parameters
f32 color[3] = {0.8f, 0.2f, 0.9f};
instance->set_parameter_float3("u_color", color);

// Integer parameters
instance->set_int("u_use_texture", 1);

// Direct binding setters
instance->set_float2(2, scale_values);
```

## Predefined Materials

### Basic Material
Simple material with time uniform for basic animations.

**Parameters:**
- `u_time` (Float) - Time value for animations

### Phong Material
Traditional Phong lighting model material.

**Parameters:**
- `u_model` (Matrix4x4) - Model transformation matrix
- `u_view` (Matrix4x4) - View transformation matrix
- `u_projection` (Matrix4x4) - Projection transformation matrix
- `u_ambient` (Float3) - Ambient light color
- `u_diffuse` (Float3) - Diffuse light color
- `u_specular` (Float3) - Specular light color
- `u_shininess` (Float) - Material shininess

### PBR Material
Physically Based Rendering material.

**Parameters:**
- `u_albedo` (Float3) - Base color/albedo
- `u_metallic` (Float) - Metallic factor (0-1)
- `u_roughness` (Float) - Roughness factor (0-1)
- `u_ao` (Float) - Ambient occlusion factor

## Migration from Hardcoded Uniforms

### Before (Hardcoded)
```cpp
// In renderer.cpp
UniformBuffer m_uniform_buffer;
WGPUBindGroup m_bind_group;

void init_bind_group() {
    // Manual bind group creation
    WGPUBindGroupEntry entry = WGPU_BIND_GROUP_ENTRY_INIT;
    entry.binding = m_uniform_buffer.binding;
    entry.buffer = m_uniform_buffer.buffer;
    // ... more manual setup
}

void update_uniforms(f32 time) {
    wgpuQueueWriteBuffer(queue, m_uniform_buffer.buffer, 0, &time, sizeof(f32));
}
```

### After (Material System)
```cpp
// In renderer.cpp
scope<MaterialInstance> m_material_instance;

void init() {
    auto material = MaterialManager::create_basic_material("BasicMaterial", shader);
    m_material_instance = material->create_instance(pipeline.get());
}

void update_uniforms(f32 time) {
    m_material_instance->set_parameter_float("u_time", time);
}

void draw() {
    m_material_instance->bind(pass_encoder);
    // ... rest of drawing
}
```

## Benefits

1. **Type Safety**: Compile-time checking of parameter types
2. **Reusability**: Materials can be reused across multiple objects
3. **Maintainability**: Centralized parameter management
4. **Flexibility**: Easy to add new parameter types and materials
5. **Performance**: Efficient GPU buffer management
6. **Debugging**: Better error messages and parameter validation

## Best Practices

1. **Use MaterialManager**: Always use the MaterialManager for creating materials
2. **Name Parameters**: Use descriptive parameter names for better code readability
3. **Set Default Values**: Always provide sensible default values for parameters
4. **Reuse Materials**: Create material templates and reuse them for similar objects
5. **Clean Up**: Call `MaterialManager::shutdown()` when done

## Integration with Existing Code

The material system is designed to be a drop-in replacement for hardcoded uniforms. The main changes needed are:

1. Replace `UniformBuffer` and `WGPUBindGroup` with `MaterialInstance`
2. Use `MaterialManager` to create materials
3. Replace manual uniform updates with material parameter setters
4. Use `MaterialInstance::bind()` instead of manual bind group binding

This provides a much cleaner and more maintainable approach to managing shader parameters in your renderer. 
