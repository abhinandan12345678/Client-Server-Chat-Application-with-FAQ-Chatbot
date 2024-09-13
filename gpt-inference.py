import sys
import time
import torch
from transformers import AutoTokenizer, AutoModelForCausalLM

start = time.time()
# Load pre-trained GPT-2 model and tokenizer
model_name = "gpt2"
# model_name = "openai-community/gpt2-medium"  # You can choose from "gpt2", "gpt2-medium", "gpt2-large", "gpt2-xl"
tokenizer = AutoTokenizer.from_pretrained(model_name)
model = AutoModelForCausalLM.from_pretrained(model_name)

# Set device to GPU if available
device = "cuda" if torch.cuda.is_available() else "cpu"
model.to(device)


# Generate text based on a prompt (same generate_text() function as before)
def generate_text(prompt, max_length=80):
    input_ids = tokenizer.encode(prompt, return_tensors="pt").to(device)
    output = model.generate(input_ids, max_new_tokens=15, pad_token_id=tokenizer.eos_token_id, no_repeat_ngram_size=3,
                            num_return_sequences=1, top_p=0.95, temperature=0.1, do_sample=True)
    generated_text = tokenizer.decode(output[0], skip_special_tokens=True)
    return generated_text


if __name__ == '__main__':
    prompt = f"""This a simple FAQ frequently asked questions about airline services the questions and answers varies 
    from greetings to booking and cancellation of flights.
    The answers are generated based on the questions asked they are simple and direct.
    Lets say one asks a question like "Good morning? " the response will be "Good morning, how may I help you?"
    Another question like "How do I book a flight?" the response will be "You can book a flight by visiting our website or calling our customer service line."
    What do you think will be the response to the question "{sys.argv[1]}?" the response will be """

    generated_text = generate_text(prompt)
    print(generated_text.split(prompt)[-1].strip())
