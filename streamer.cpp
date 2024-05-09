#include "streamer.h"

void sc_streamer_init(struct sc_streamer* streamer, const char* name) {
    // Initialize other members as needed
    streamer->name = name;
    streamer->window_name = "Frame Display"; // Set OpenCV window name
    streamer->display_window = true; // Enable window display by default
}

void sc_streamer_display_frames(struct sc_streamer* streamer) {
    // Check if the frame sink is valid
    if (!streamer->frame_sink || !streamer->frame_sink->ops->push)
        return;

    // Create an OpenCV window
    cv::namedWindow(streamer->window_name, cv::WINDOW_AUTOSIZE);

    while (streamer->display_window) {
        // Get the frame from the frame sink
        AVFrame* frame = (AVFrame*)streamer->frame_sink->ops->push(streamer->frame_sink,nullptr);

        // Convert the frame to an OpenCV Mat object
        cv::Mat image(frame->height, frame->width, CV_8UC3, frame->data[0]);

        // Display the frame
        cv::imshow(streamer->window_name, image);

        // Wait for a key press
        int key = cv::waitKey(1);

        // If 'q' key is pressed, exit the loop
        if (key == 'q') {
            streamer->display_window = false;
            break;
        }
    }

    // Close the OpenCV window
    cv::destroyWindow(streamer->window_name);
}