# Use the official Python 3.12 image as the base
FROM python:3.12

# Set the working directory in the container to /app
WORKDIR /app

# Copy the current directory contents into the container at /app
COPY ./ml /app/ml

# Install virtualenv
RUN python -m venv /app/ml/venv

# Install the necessary packages inside the virtual environment
RUN /app/ml/venv/bin/pip install tensorflow==2.16.1 \
  music21==9.1.0 \
  keras==3.3.3

# Set the working directory to the /app/ml directory
WORKDIR /app/ml

# Activate the virtual environment and run the sample.py script with the argument 30
CMD ["/bin/bash", "-c", "source /app/ml/venv/bin/activate && python sample.py 30"]
