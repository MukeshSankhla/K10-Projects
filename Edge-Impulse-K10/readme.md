Edge AI is becoming increasingly important as more applications demand real-time intelligence directly on embedded devices. With this in mind, the **UNIHIKER K10** stands out as a powerful new single-board computer designed for education, prototyping, and AI-based projects. It comes packed with onboard peripherals such as a **camera, microphone, display, speaker, buttons, and multiple sensors**, making it an excellent platform for learning and experimenting with Edge AI.

However, being a **newly released board**, the UNIHIKER K10 presents a unique challenge. DFRobot provides a **pre-built UNIHIKER K10 library** that simplifies access to all the onboard peripherals. While this makes the board very easy to use for beginners, it also limits **low-level hardware control**. This becomes a major obstacle when trying to build **custom applications**, especially advanced workflows like **integrating Edge Impulse with custom sensor pipelines and camera handling**.

In this tutorial, we go beyond the default library approach. We not only demonstrate **how to interface Edge Impulse with the UNIHIKER K10 and use its sensors for Edge AI applications**, but we also **reverse-engineer the board** to gain deeper control over the hardware. By studying the **official datasheet and hardware architecture**, we implement our **own custom code**, enabling advanced Edge Impulse use cases that are not possible using the standard libraries alone.

By the end of this project, you will understand:
1.   How to run **Edge AI models on the UNIHIKER K10**
2.   How to work with its **camera and sensors for Edge Impulse**
3.   How to bypass library limitations using **datasheet-based custom coding**
4.   How to unlock the board’s full potential for **real-world Edge AI applications**

This guide is ideal for makers, students, and developers who want to move beyond basic examples and truly explore **low-level Edge AI development on the UNIHIKER K10**.

For beginner, in this tutorial, you’ll learn how to build **real Edge AI models** on the **Unihiker K10** using **Edge Impulse**.

We’ll walk through:
- IMU gesture classification
- Environmental sensor regression
- Audio classification
- Image object detection

By the end, your K10 will be running multiple AI models directly on the device — no cloud processing needed!

GitHub Repo: [https://github.com/MukeshSankhla/Learn-Edge-AI-on-Unihiker-K10-Edge-Impulse-Beginner-Tutorial-](https://github.com/MukeshSankhla/Learn-Edge-AI-on-Unihiker-K10-Edge-Impulse-Beginner-Tutorial-)

![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/1.png)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/2.gif)

## Supplies
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/3.JPG)
- 1x [Unihiker K10 Kit](https://www.dfrobot.com/product-2904.html)
- 1x SD Card
- 1x 3D Printed Case (Optional)

** Software apps and online services**
- [Edge Impulse Studio](https://www.hackster.io/EdgeImpulse/products/edge-impulse-studio?ref=project-33fef7)
- [Arduino IDE](https://www.hackster.io/arduino/products/arduino-ide?ref=project-33fef7)

![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/4.JPG)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/5.JPG)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/6.JPG)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/7.JPG)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/8.JPG)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/9.JPG)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/10.JPG)


1. Download the Project GitHub Repository
- Go to the project’s GitHub page: [Learn-Edge-AI-on-Unihiker-K10-Edge-Impulse-Beginner-Tutorial ](https://github.com/MukeshSankhla/Learn-Edge-AI-on-Unihiker-K10-Edge-Impulse-Beginner-Tutorial-)
- Click Code → Download ZIP.
- Extract the ZIP file to your computer.

2. Copy the Assets Folder to the SD Card
- Inside the extracted project folder, locate the: assets folder
- This folder contains required images
- Insert the SD card used with the Unihiker K10 into your computer.
- Copy the entire assets folder into the root directory of the SD card.

Download the **[Case.stl](https://content.instructables.com/FNC/FM56/MJAQF5OA/FNCFM56MJAQF5OA.stl?_gl=1*a4w804*_ga*MTg5ODg0MTkxNS4xNzUyNzU2OTk5*_ga_NZSJ72N6RX*czE3NjYzOTE1MDUkbzI0OSRnMSR0MTc2NjM5MzAxNSRqNTYkbDAkaDA.)** and 3D print it, or open the [Unihiker Case](https://a360.co/4s4IZ9f) in Fusion 360 if you want to make changes.

## Step 1: Introduction to UNIHIKER K10
------------------------------------

![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/11.webp)

The UNIHIKER K10 is an all-in-one AI learning board designed for beginners, makers, and educators who want to explore Edge AI, TinyML, sensors, and IoT without any wiring or extra modules.

**Key Built-In Hardware**
- ESP32-S3 microcontroller (TinyML-ready)
- 2.8" Color Display (240×320)
- 2MP Camera (perfect for image classification & object detection)
- Dual MEMS Microphones + Speaker (sound classification, anomaly detection, voice features)

**Sensors:**
- Temperature & Humidity
- Ambient Light
- 6-Axis IMU (motion & gesture detection)
- RGB LED, Buttons, MicroSD Slot, GPIO Expansion
- Wi-Fi + Bluetooth connectivity

Everything you need for AI experiments is already on the board — no extra hardware required.

**Built-In AI Features**
The K10 ships with ready-to-use demos like:
- Face detection
- Object recognition
- QR code detection
- Motion detection
- Offline voice commands

These give you an instant taste of what the hardware can do.

**Why It’s Perfect for Edge Impulse**
- Onboard sensors make data collection effortless
- Runs lightweight ML models directly on the device

In this tutorial, we’ll use all these onboard features to build real AI projects step-by-step.

## Step 2: Introduction to Edge Impulse
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/12.png)

Edge Impulse is a platform that makes it easy to build and deploy machine learning models on edge devices—like the UNIHIKER K10, without needing deep AI expertise.
It’s designed for beginners, makers, and engineers who want to turn sensor data → trained ML model → real-world application in just a few steps.

**Why Edge Impulse?**
- **No AI background needed:** Build models using a visual workflow.
- **Works with any sensor:** IMU, microphone, camera, environment sensors and more...
- **Collect data directly from the device:** Record motion, sound, images, or environmental readings in real time.
- **Automated ML pipeline:** Feature extraction, model training, tuning—handled for you.
- **Easy deployment:** Export models directly as firmware, libraries, or code you can run on the K10.

## Step 3: Core Concepts of Edge Impulse

To build any ML project in Edge Impulse, you’ll work through a simple workflow:
**Acquire Data → Build an Impulse → Train → Deploy**

Here’s what each part means and how you’ll use it in this tutorial.

#### 3.1. Data Acquisition (Collecting Data)
Machine learning always starts with good, clean data.
Edge Impulse supports multiple ways to bring data into your project.
In this tutorial, we’ll cover these three:
*   **Data Forwarder**: Streams live sensor values (IMU, temperature, humidity, light, etc.) from the K10 to Edge Impulse. Great for collecting gesture, environmental, or motion data.
*   **CSV Wizard**: Upload CSV files and automatically map columns to sensors. Useful for any data you already logged or generated separately.
*   **Data Upload**: Drag-and-drop images, audio files, or sensor logs directly. Perfect for camera datasets or pre-recorded sound samples.

These three methods give you flexibility no matter what sensor or project you’re working on.

#### 3.2. Creating an Impulse
An Impulse is your complete ML pipeline — the recipe that explains how data flows from input → preprocessing → model → output.
An Impulse contains:
- Input block (sensor data, audio, images, etc.)
- Processing block (feature extraction)
- Learning block (the model you train)
- Output block (classification, prediction, score, detection)

#### 3.3. Understanding the Blocks
*   **Time-Series Blocks**: For IMU, environmental sensors, or audio. Generates features like frequency, energy, motion patterns.
*   **Spectrogram / MFCC**: For audio: Converts sound into frequency images that the model can learn from.
*   **Image Processing**: For camera images: Resizes, normalizes, and prepares data for object detection or classification.
*   **Neural Network / Regression / Anomaly Blocks**: Models that learn patterns and make predictions.

#### 3.4. Types of Models in Edge Impulse
*   **Classification**: Identifies which category something belongs to. (Gestures, Sound types, Simple image classes)
*   **Regression**: Predicts a continuous value. (Temperature/light prediction, Sensor trend estimation)
*   **Anomaly Detection**: Finds "something unusual" compared to normal behavior. (Identifying abnormal movements, Monitoring environmental changes)
*   **Object Detection**: Locates and identifies objects in images (bounding boxes). (Counting objects, Detecting tools or items)

#### 3.5. Training the Model
Once your impulse is set, you train your model with your collected data:
- Choose training/test split
- Tune hyperparameters (automatically or manually)
- Check accuracy and performance metrics
- Visualize loss curves and confusion matrix

#### 3.6. Deploying the Model
After training, Edge Impulse lets you deploy in multiple ways (Firmware, Library, WebAssembly, etc.).
For this tutorial, you’ll deploy models as an **Arduino Library** for the Unihiker K10.

## Step 4: Motion Data Classification
--------------------------------------
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/13.png)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/14.png)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/15.png)

From Step 4 to Step 10, we will train and deploy a **Motion Classification model** on the Unihiker K10 using Edge Impulse.

- To begin, go to [edgeimpulse.com](http://edgeimpulse.com).
- If you are a new user, create an account and log in.
- Once logged in, you will see your Project Dashboard.
- Click **Create new project**, enter a project name, select Public or Private, and click Create.

## Step 5: Prepare for Data Acquisition
----------------------------------------

![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/16.png)

In this step, we will collect 6-axis IMU data from the Unihiker K10 into the Edge Impulse project using the Data Forwarder method.

#### 5.1 Flash the Data-Streaming Arduino Code
- Download the GitHub repo for this project: [Learn-Edge-AI-on-Unihiker-K10-Edge-Impulse-Beginner-Tutorial](https://github.com/MukeshSankhla/Learn-Edge-AI-on-Unihiker-K10-Edge-Impulse-Beginner-Tutorial-)
- Open the `Motion_Data_Collect` sketch in the Arduino IDE.
Before uploading the code, ensure that the Unihiker K10 board manager is installed.

![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/17.png)

#### 5.2 Install the Unihiker K10 Board in Arduino IDE
- Open `File → Preferences`.
- In Additional Board Manager URLs, paste: 
```c
https://downloadcd.dfrobot.com.cn/UNIHIKER/package_unihiker_index.json
```
- Click OK.
- Go to `Tools → Board → Board Manager`.
- Search for **Unihiker** and install it.

![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/18.png)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/19.png)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/20.png)

#### 5.3 Connect and Upload the Code
- Connect the Unihiker K10 to your PC via USB.
- Go to Tools:
    - Select Board: Unihiker K10
    - Select the correct COM port
    - Enable USB CDN on Boot
- Upload the sketch.
Once the upload is successful, the K10 is ready to stream IMU data over USB.

![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/21.png)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/22.png)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/23.png)

#### 5.4 Install Edge Impulse CLI
To send data from the device to Edge Impulse, install the CLI on your PC: [Edge Impulse CLI Installation Guide](https://docs.edgeimpulse.com/tools/clis/edge-impulse-cli/installation)

After installation, open a new Command Prompt window and run:
```bash
edge-impulse-data-forwarder
```

The CLI will ask you to log in using your Edge Impulse email and password.

![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/24.png)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/25.png)

#### 5.5 Select Your Device and Project
- Select the COM port used by the Unihiker.
- Select the project you created for this tutorial.

![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/26.png)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/27.png)

#### 5.6 Naming the Sensor Parameters
The CLI will ask for CSV column names. For IMU data, name them: `x, y, z`
Next, enter a device name.

![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/28.png)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/29.png)

#### 5.7 Start Collecting Data
Do not close the Command Prompt window. Now return to your browser.

## Step 6: Data Collection in Edge Impulse

![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/30.gif)

- Open your Edge Impulse project and go to **Data Acquisition**.
- You should see your device listed under “Connected Devices.”
- Set the Label (e.g., `idle`, `shake`, `left_right`, `up_down`) and Sample Length.
- Click **Start Sampling**.

**Gestures to Record:**
- **Idle** (keep the device still)
- **Shake**
- **Left-Right (LR)** motion
- **Up-Down (UD)** motion

![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/32.png)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/33.png) 
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/34.png)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/35.png)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/36.png)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/37.png)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/38.png)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/39.png)

- Record 3 samples × 10 seconds each → 30 seconds of data per gesture.
- After collecting, click the three dots next to each 10-second sample and choose **Split sample** into 1-second windows.
- Move a few samples from Training to Testing (80:20 split).

![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/31.gif)

![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/40.png)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/41.png)

## Step 7: Create Impulse

![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/42.png)

- Go to **Create Impulse**.
- Add blocks:
    - **Processing Block**: Spectral Analysis (captures motion frequency patterns).
    - **Learning Block**: Classification.
- Click **Save Impulse**.

![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/43.png)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/44.png)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/45.png)

## Step 8: Generate Features   

![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/46.png)

- Go to **Spectral Features**.
- Keep default parameters and click **Save parameters**.
- Switch to **Generate Features** and click **Generate features**.
- You’ll see a Feature Explorer plot showing clusters for each gesture.

![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/47.png)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/48.png)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/49.png)

## Step 9: Train the Model

![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/50.png)

- Go to the **Classifier** panel.
- Keep default settings.
- Click **Save & Train**.
- Review Accuracy, Loss, Confusion Matrix, and Performance Metrics.

![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/51.png)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/52.png)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/53.png)

## Step 10: Deploying the Model on Unihiker K10

![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/54.png)

1.  **Export the Model as Arduino Library**:
    - Go to **Deployment**.
    - Choose **Arduino Library**.
    - Click **Build**. Download the .zip file.

![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/55.png)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/56.png)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/57.png)

2.  **Install the Arduino Library**:
    - In Arduino IDE: `Sketch → Include Library → Add .ZIP Library…`

![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/59.png)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/60.png)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/61.png)

3.  **Import the Example Code**:
    - Open `Motion_Data_Classification.ino` from the GitHub repo.

![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/58.png)

4.  **Fix the Library Name**:
    - Replace `#include <your_ei_library_name_here.h>` with the actual name from the installed library example.

![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/62.png)

5.  **Upload to Unihiker K10**:
    - Select Board: Unihiker K10, correct COM port.
    - Click **Upload**.

![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/63.png)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/64.png)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/65.png)

The code maps gestures to RGB Colors and Emojis:
- **Idle** → Green + Normal face
- **UD** → Pink + Happy emoji
- **Shake** → Red + Spiral eyes
- **Left/Right** → Blue + Sleepy emoji

![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/2.gif)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/66.gif)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/67.gif)

## Step 11: Environmental Sensor Regression

Steps 11-14 cover training and deploying an **Environmental Sensor Regression model** to predict a "Comfort Score".

#### 11.1. Create a New Edge Impulse Project
Start a new project for regression.

![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/68.png)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/69.png)

#### 11.2. Generating a Dataset
Use the dataset generator web app: [Dataset Generator](https://claude.ai/public/artifacts/c39715ad-48e6-4e8b-ad16-cbb41714074a)
- Configure Min/Max Temp & Humidity.
- Generate and download CSV.

![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/70.png)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/71.png)

#### 11.3. Import Data Using CSV Wizard
- In Edge Impulse, go to **Data Acquisition → CSV Wizard**.
- specific "No time-series data", Label: "Comfort score", Features: "temperature", "humidity".

![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/72.png)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/73.png)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/74.png)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/75.png)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/76.png)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/77.png)

#### 11.4. Upload the Data
Upload the CSV data.

![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/78.png)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/79.png)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/80.png)

## Step 12: Create Impulse & Generate Features
#### 12.1. Create the Impulse
- Processing Block: **Raw Data**.
- Learning Block: **Regression**.
- Click **Save impulse**.

![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/81.png)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/82.png)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/83.png)

#### 12.2. Generate Features
- Go to **Raw data** → **Save parameters** → **Generate features**.

![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/84.png)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/85.png)

## Step 13: Train the Regression Model & Understand the Results

Now that all features are generated, the next step is to train the regression model that predicts the **Comfort Score** based on **Temperature** and **Humidity**.

**1. Train the Model**

![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/86.png)

- Go to the **Regression** panel in Edge Impulse.
- Leave all settings at their **default values**.
- Click **Save & Train**.

![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/87.png)

Edge Impulse will now train a lightweight neural network regression model.
Once training is complete, you will see the model performance results.

**2. Understanding the Training Results**

![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/88.png)

After training, Edge Impulse provides two important visual reports:

**Model Metrics (Validation Set)**

These metrics indicate how accurate the model is:

**Loss (MSE): 0.42**

Low mean squared error → very good accuracy

**Mean Absolute Error (MAE): 0.51**

Predictions are off by only ±0.5 on average

**Explained Variance Score: 0.93**

Model explains 93% of the variation → excellent

**Interpretation**

- A **0.42 MSE** and **0.93 variance score** indicate that the model fits the data very well.
- The small MAE means comfort scores are predicted very close to the real values.
- Overall, this is a **stable and highly accurate regression model** ready to be deployed.

**Data Explorer (Prediction Visualization)**

The scatter plot shows how well the model predicts values across the entire dataset.

- **Green dots** – Predictions that are **correct** (within the acceptable error threshold).
- **Red dots** – Predictions that are **incorrect** (outside the error threshold).

From the visualization, we can observe:

- Most data points are **green**, meaning the model learned the relationship between Temperature, Humidity, and Comfort Score very well.
- Only a few points are **red**, usually in extreme environmental ranges.
- This indicates a **high-quality regression model** suitable for real-time applications.

Edge Impulse also lists the **on-device performance**:

- **Inferencing time:** ~2 ms
- **Peak RAM usage:** ~1.2 KB
- **Flash usage:** ~10.2 KB

This confirms the model is extremely lightweight and ideal for deployment on Unihiker K10.

**3. Export the Model**

Now go to the **Deployment** section:

- Select **Arduino Library**
- Click **Build** to generate the library
- Edge Impulse will automatically download the **.zip** file

This library will be used in the next step to run comfort score predictions directly on the **Unihiker K10**.

## Step 14: Upload & Run the Regression Model on Unihiker K10

Now let’s deploy the Comfort Score Regression model on the Unihiker K10.

**14.1. Open the Arduino Example Code**

- From the downloaded GitHub repository, open the
- **Environmental_Sensor_Regression** sketch in **Arduino IDE**.

![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/90.png)

**14.2. Install Required Libraries**

**a. Install the Edge Impulse Regression Model Library**

- Go to **Sketch → Include Library → Add .ZIP Library**
- Select the Arduino library you downloaded from Edge Impulse.
- Once installed, the library will appear under **File → Examples**.

**Important:** Update the Edge Impulse library name in the code to match the _exact name_ of your downloaded library folder.

**b. Install the AHT20 Sensor Library**

- Open **Library Manager** (Tools → Manage Libraries)
- Search for **AHT20**
- Install the official **AHT20** temperature & humidity sensor library.

![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/91.png)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/92.png)

**14.3. Upload the Code to Unihiker K10**

- Select the correct **Board** (Unihiker K10)
- Select the correct **COM port**
- Click **Upload**

![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/93.png)

The first compilation may take some time—be patient.

**14.4. View the Results on the Screen**

![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/89.JPG)

Once uploaded successfully, the K10 display will show:

- **Live Temperature** readings
- **Live Humidity** readings
- **Predicted Comfort Score** (from the regression model)

The values update in real time, demonstrating on-device machine learning running directly on the Unihiker K10.

## Step 15: Audio Classification – Data Collection

In **Step 15 to Step 17**, we will collect audio data using the Unihiker K10, train an **audio classification model**, and deploy it on the device.

#### **1. Create a New Edge Impulse Project**

- Go to **edgeimpulse.com**
- Create a **new project** for audio classification.

![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/94.png)

#### **2. Upload Audio Data Collection Firmware**

- Open the **Audio_Data_Collect** Arduino sketch from the GitHub repository.
- In the code:
- Set the **number of labels**
- Define the **label names** (classes)

![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/95.png)

```c
// ---------------- Settings -----------------
String labels[] = {"Noise", "Gun", "Chainsaw"};
int labelCount = 3;
int currentLabelIndex = 0;
```

- Upload the code to the **Unihiker K10**.

#### **3. Audio Recording Interface on K10**

Once uploaded, the K10 screen will display:

- Current **label name**
- **Sampling time** (in seconds)
- Name of the **last recorded audio file**

Each audio file is saved as: LabelName_RandomNumber.wav

#### **4. Button Controls**

![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/96.gif)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/97.gif)

The onboard buttons control recording and navigation:

**Button A**
- Click → Start audio recording
- Long press → Go to previous label

**Button B**
- Click → Increase recording time by 1 second
- Double click → Reset recording time to default
- Long press → Go to next label

#### **5. Collect Audio Samples**

For this demo, record the following classes:

- **Noise** (background sound)
- **Gunshot**
- **Chainsaw**

**Data Collection Plan**

- Each recording: **10 seconds**
- Total data per class: **~1 minute**

This provides enough variation for training a reliable audio classification model.

## Step 16: Audio Data Processing**

![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/98.png)

Once the audio samples are recorded and saved on the SD card, the next step is to import and prepare the data in Edge Impulse.

#### **1. Upload Audio Data**

![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/99.png)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/100.png)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/101.png)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/102.png)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/103.png)

- Open your **Edge Impulse project**
- Go to **Data Acquisition → Upload data**
- Select all audio files belonging to **one label**
- Set the **Label name** correctly
- Upload the files

Repeat this for **each audio class**.

#### **2. Split Audio Samples**

![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/104.png)

- After uploading, each audio file will be **10 seconds long**
- For better training, split each sample into **1-second segments** (Both in training and test)
- This increases the number of training samples and improves accuracy

#### **3. Crop Important Audio Features**

![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/105.png)

- While splitting, make sure to **crop around meaningful audio spikes**
- Remove silent or irrelevant portions of the audio
- Focus on sections where the sound pattern is clearly visible

![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/106.png)

## Step 17: Training the Audio Classification Model

Once your audio dataset is clean and properly split, it’s time to train and deploy the model.

#### **1. Create the Impulse**

![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/107.png)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/108.png)

- Go to **Create Impulse**
- Set:
	- **Processing block**
	- **MFE** → for non-voice sounds (noise, machines, events)
	- **MFCC** → for voice or speech-based data
	- **Learning block** → **Classification**
- Leave all other settings as default
- Click **Save Impulse**

#### **2. Generate Features**

![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/109.png)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/110.png)

- Open the **MFE / MFCC** panel
- Click **Save parameters**
- Click **Generate features** (use default settings)

This step extracts meaningful frequency features from the audio samples.

#### **3. Train the Model**

![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/111.png)

- Go to the **Classification** panel
- Keep all parameters at their default values
- Click **Save & Train**

#### **4. Review Model Results**

![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/112.png)

Once training is complete, Edge Impulse will display:

- Overall **model accuracy**
- **Confusion matrix** showing how well each class is recognized
- Per-class performance metrics

If the accuracy is low, consider collecting more data or improving audio cropping.

#### **Model Results Explanation**

From the results shown above, we can see that the audio classification model is performing extremely well:

**Accuracy: 100%**

- The model correctly classified all validation samples.

**Confusion Matrix**
- Chainsaw, Gun, and Noise are all classified with **100% confidence**
- No overlap or misclassification between classes
- This means each sound has a very distinct audio pattern.

**Evaluation Metrics**
- Precision, Recall, and F1 Score are all **1.0**
- Area under ROC Curve is **1.0**, indicating perfect separability
- Data Explorer
- Each class forms a **clearly separated cluster**
- This shows the MFE/MFCC features successfully captured unique sound characteristics
**On-device Performance**
- Inference Time: **7 ms**
- RAM Usage: **14.7 KB**
- Flash Usage: **41 KB**

This confirms the model is **accurate, lightweight, and perfectly suited for real-time edge deployment on the Unihiker K10**.

## Step 18: Image Object Detection – Data Collection

In **Step 18 to Step 20**, we will collect image data, train an **image object detection model**, and deploy it on the Unihiker K10.

#### **1. Create a New Project**

![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/113.png)

Go to **Edge Impulse**
Create a **new project** for image object detection

#### **2. Upload Image Data Collection Firmware**
- Open the **Image_Data_Collect** Arduino sketch from the GitHub repository
- In the code:
- Set the **number of labels**
- Define the **label names**

```c
/* =========================================================
Labels Configuration
========================================================= */
String labels[] = {"BACKGROUND","IN", "CN", "USA", "HK"};
int labelCount = 5;
int currentLabelIndex = 0;
```
- Upload the code to the **Unihiker K10**

#### **3. Image Capture Controls**

![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/114.gif)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/115.JPG)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/116.JPG)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/117.JPG)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/119.JPG)

Once the code is uploaded, use the buttons to capture images:

- **Button A** → Capture image
- **Button B** → Next label
- **Long press Button A** → Previous label

![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/118.gif)

Each captured image is saved as:
```c
<LabelName>_<RandomNumber>.jpeg
```
#### **4. Collect Image Dataset**

For this demo, images were collected for **4 different coins**:

- Indian Coin
- Chinese Coin
- Hong Kong Coin
- USA Coin

#### **5. Background Images**

Background-only images (without objects) help the model learn what **not** to detect
These improve detection accuracy and reduce false positives

**Data Collection Tips**

- Capture **15–16 images per class**
- Use **different orientations, angles, and lighting**
- Ensure the object is clearly visible in each frame

#### **6. Upload Images**

- Copy images from the SD card
- Upload all images to **Data Acquisition** in Edge Impulse
- Upload **without assigning labels** (labels will be added during bounding box annotation)

## Step 19: Box Annotation

![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/120.gif)

After uploading all images, the next step is to label the objects using **bounding box annotation**.

#### **1. Annotate Images**

![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/121.png)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/122.png)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/123.png)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/124.png)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/125.png)

- Open **Data Acquisition**
- Click on each uploaded image
- Draw a **bounding box** around the object (coin)
- Assign the correct **label name**

Make sure the box tightly covers only the object.

#### **2. Annotate All Samples**

- Repeat this process for **all images**
- Annotate both **training** and **test** datasets

Once all images are annotated, your dataset is ready for **impulse creation and model training** in the next step

## Step 20: Training the Image Object Detection Model

Now that all images are annotated, we can create and train the object detection model.

#### **1. Create the Impulse**

![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/126.png)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/127.png)

- Go to **Create Impulse**
- Set:
	- **Processing block** → **Image**
	- **Learning block** → **Object Detection**
- Leave all other settings as default
- Click **Save Impulse**

#### **2. Configure Image Processing**

![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/128.png)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/129.png)

- Open the **Image** block
- Select **Grayscale** (reduces memory usage and improves edge performance)
- Click **Save parameters**
- Click **Generate features**

#### **3. Train the Object Detection Model**

![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/130.png)

Go to the **Object Detection** panel
Change the following parameters:
- **Training cycles:** 60
- **Learning rate:** 0.01

Leave other settings unchanged
Click **Save & Train**

#### **4. Model Results Explanation**


![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/131.png)

From the results shown above:

**F1 Score: 96%**

Indicates strong balance between precision and recall for object detection.

**Confusion Matrix**

Coin from **India, China, Hong Kong, and USA** are detected correctly

Background samples help reduce false detections

**Metrics**

**Precision (non-background): 1.00** → Very few false positives

**Recall (non-background): 0.92** → Most objects are detected successfully

**F1 Score: 0.96** → Excellent real-world performance

**On-device Performance**

Inference Time: ~**1.1 seconds**

RAM Usage: **119 KB**

Flash Usage: **81 KB**

This confirms the model is accurate and optimized for **edge object detection on the Unihiker K10**.

## Step 21: UNIHIKER K10 – Detailed Pin Mapping, I²C Addresses & Control Lines

![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/132.png)

This section describes how each major peripheral on the **UNIHIKER K10** is electrically connected to the **ESP32-S3**, including **exact GPIO pins, I²C addresses, and signal roles**, as observed from the schematic and validated by practical code mapping.

#### **1. Core MCU – ESP32-S3-WROOM-1**

The ESP32-S3 acts as the central controller and directly interfaces with **camera, audio (I2S), SPI peripherals, USB, and I²C devices**.

## Step 22: Camera Interface (GC2145 – Parallel DVP)

![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/133.png)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/134.png)

**Camera Sensor:** GC2145
**Interface Type:** 8-bit parallel DVP + I²C control
Camera Data & Sync Pins
Camera Control

## Step 23: Display Interface (ILI9341 – SPI)

![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/135.png)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/136.png)

**Display Controller:** ILI9341
**Interface:** SPI (no MISO)
SPI alone is insufficient - **display initialization requires XL9535 configuration first.**

## Step 24: SD Card (SPI Mode)

![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/137.png)

**Interface:** SPI (shared bus)

## Step 25: Audio Subsystem (I2S)

![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/138.png)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/139.png)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/140.png)

I2S Signal Mapping
#### **Microphone ADC – ES7243E**
**Function:** Digital microphone ADC
**Interface:** I2S + I²C
#### **Speaker Amplifier – NS4168**

## Step 26: I²C Bus Devices (Shared Bus: GPIO47 / GPIO48)
![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/141.png)

## Step 27: GPIO Expander – XL9535 (Critical Component)

![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/142.png)

**IC:** XL9535QF24
**Address:****0x20**
Functions Controlled via XL9535

## Step 28: Buttons – Button a & Button B

![Project Cover](https://github.com/makerbrains/Edge-Impulse-K10/raw/master/images/143.png)

Buttons are **NOT connected directly** to ESP32 GPIOs.

## Conclusion
In this tutorial, you learned how to build **end-to-end Edge AI applications** on the **Unihiker K10** using **Edge Impulse**.

We covered:
1.   **IMU-based gesture classification -**[Motion_Data_Classification](https://studio.edgeimpulse.com/public/850336/live)
2.   **Environmental sensor regression** for comfort prediction
3.   **Audio classification** using the onboard microphone - [Audio_Data_Classification](https://studio.edgeimpulse.com/public/850665/live)
4.   **Image object detection** using the camera - [Image_Data_Object Detection](https://studio.edgeimpulse.com/public/860042/live)

You explored the complete workflow — from **data collection and preprocessing** to **model training, optimization, and deployment** as an **Arduino library** running directly on the device with real-time inference and on-screen visualization.

This demonstrates how powerful and accessible **Edge AI** has become — enabling anyone to turn sensor data into intelligent, responsive applications without cloud dependency.

**What’s next?**

1.   Add more sensors or classes
2.   Improve models with more data
3.   Combine multiple models into a single smart system
4.   Build real-world applications like smart assistants, safety systems, or interactive devices

Thanks for following along, and happy building with **Edge Impulse on Unihiker K10**!
