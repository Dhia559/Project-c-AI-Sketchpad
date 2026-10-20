import sys
import torch
from diffusers import StableDiffusionXLImg2ImgPipeline
from PIL import Image
import os

print("=" * 60)
print("Loading JuggernautXL Ragnarok (SDXL)")
print("=" * 60)

# Get the directory where this script is located
script_dir = os.path.dirname(os.path.abspath(__file__))
model_path = os.path.join(script_dir, "juggernautXL_ragnarokBy.safetensors")

if not os.path.exists(model_path):
    print(f"ERROR: Model not found at {model_path}")
    sys.exit(1)

print(f"Model found: {model_path}")
print("Loading model... (this takes 30-60 seconds)")

try:
    pipe = StableDiffusionXLImg2ImgPipeline.from_single_file(
        model_path,
        torch_dtype=torch.float16,
        use_safetensors=True,
        local_files_only=False,
    )
    
    device = "cuda" if torch.cuda.is_available() else "cpu"
    print(f"Using device: {device}")
    
    if device == "cpu":
        print("WARNING: Running on CPU will be VERY slow (5-10 minutes)")
        pipe = pipe.to("cpu")
        pipe.enable_attention_slicing()
    else:
        pipe = pipe.to("cuda")
        pipe.enable_model_cpu_offload()
    
    print("Model loaded successfully!")
    
    input_path = sys.argv[1]
    print(f"Loading sketch from: {input_path}")
    
    init_image = Image.open(input_path).convert("RGB")
    init_image = init_image.resize((1024, 1024), Image.Resampling.LANCZOS)
    
    prompt = "masterpiece, ultra high quality, ultra detailed, 8k, cinematic lighting, professional artwork, best quality, sharp focus"
    
    negative_prompt = """blurry, low quality, sketch, draft, rough, unfinished, 
    messy, pixelated, jpeg artifacts, ugly, deformed, bad anatomy"""
    
    print("Generating enhanced image...")
    
    result = pipe(
        prompt=prompt,
        negative_prompt=negative_prompt,
        image=init_image,
        strength=0.75,
        guidance_scale=12,
        num_inference_steps=50,
    ).images[0]
    
    # Save in the same directory as the script
    output_path = os.path.join(script_dir, "enhanced_output.png")
    result.save(output_path)
    
    print("=" * 60)
    print(f"SUCCESS! Saved to: {output_path}")
    print("=" * 60)
    
except Exception as e:
    print(f"ERROR: {e}")
    import traceback
    traceback.print_exc()
    sys.exit(1)
