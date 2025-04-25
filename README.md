🧠 Natural Language Minesweeper (Unreal Engine Plugin)
This is a Slate-based Minesweeper widget plugin for Unreal Engine that integrates natural language input using the Model Context Protocol (MCP) and connects to a real LLM backend (DeepSeek AI) to interpret prompts like:

"Start a 10x10 game with 20 bombs"

✨ Features
🧩 Classic Minesweeper gameplay in the Unreal Editor via SCompoundWidget

🤖 Natural language input powered by DeepSeek AI

⚙️ Automatically parses width, height, and bomb count from LLM responses (MCP-style)

📦 Fully self-contained editor tool—ideal for demonstrating AI-assisted UX workflows

🔍 How It Works
When a user types a prompt into the provided text box, the widget:

Sends the prompt to a connected LLM (DeepSeek AI) using HTTP POST.

Parses the result using a Model Context Protocol-inspired format, e.g., width=10 height=10 mines=20.

Rebuilds the Minesweeper grid with the new parameters.

🧠 Why AI Was Used
This plugin was developed with the help of AI tools including:

ChatGPT (OpenAI): for initial prototyping, refactoring, and generating helper methods and boilerplate

DeepSeek AI: to process and parse natural language input into structured game parameters

I believe thoughtful use of AI speeds up iterations, encourages more creative workflows, and makes tool development more intuitive for users.Furthremore, there are very minimal chances that there would be any human errors in the codebase. It not only help to prototye fast but also save cost by allowing developer less time in logic building and more time to strong the core of underlying arcitcture of the product or tool.

🚀 Setup
Add your DeepSeek API key in SendPromptToLLM():

Request->SetHeader("Authorization", "Bearer YOUR_API_KEY");
Add this plugin to your Unreal Engine project's plugin directory.

Open the widget or call it from your editor tool code to see the interactive Minesweeper grid.

📦 Example Prompt
"Start a 5x7 game with 10 bombs"
Expected LLM response (in MCP format):

width=5 height=7 mines=10
📄 Files
SMinesweeperWidget.h/.cpp: Core Slate widget with grid generation, bomb logic, and LLM integration

MCP-style response parsing logic (ParseAndApplyMCPResponse)

HTTP communication setup with DeepSeek via Unreal's FHttpModule

📜 License
MIT License. You are free to modify, distribute, and build upon this for personal or commercial use.

