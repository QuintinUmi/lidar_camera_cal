#include "ros/ros.h"
#include "std_msgs/String.h"
#include <iostream>
#include <vector>
#include <string>
#include <termios.h>
#include <unistd.h>
#include <limits>
#include <cstdio> // for printf and getchar
#include <sys/select.h>
#include <chrono>
#include <thread>
#include <sys/ioctl.h>

#include "tools/file_operator.h"
#include "tools/CommandHandler.h"

#define KEY_UP 65
#define KEY_DOWN 66
#define KEY_ENTER 10
#define KEY_ESC 27
#define KEY_BACKSPACE 127

using namespace lcr_cal;

void enableRawMode() {
    termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag &= ~(ICANON | ECHO);  
    term.c_cc[VMIN] = 1; 
    term.c_cc[VTIME] = 0;  
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

void disableRawMode() {
    termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag |= ICANON | ECHO;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &term);
}

int getch() 
{
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}


bool kbhit() {
    int bytesWaiting;
    ioctl(STDIN_FILENO, FIONREAD, &bytesWaiting);
    return bytesWaiting > 0;
}

// Function to read key presses, including handling of escape sequences
int readKey() {
    char c;
    int read_t = read(STDIN_FILENO, &c, 1);

    if (c == '\033') {  // Escape character
        if (kbhit()) {  // Check if another character is available
            char seq[2];
            int read_t = read(STDIN_FILENO, &seq, 2);  // Read the next two bytes

            if (seq[0] == '[') {
                switch (seq[1]) {
                    case 'A': return 'A';  // Up arrow
                    case 'B': return 'B';  // Down arrow
                    default: return '\033';
                }
            }
        } else {  // No additional characters means it's a standalone ESC
            return '\033';
        }
    } else if (c == '\n') {
        return 'E';  // Enter key
    }

    return c;  // Return the read character
}


int nonBlockingGetch() {
    struct timeval tv;
    fd_set readfds;
    tv.tv_sec = 0;
    tv.tv_usec = 10000;  // 设置超时为10毫秒

    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);

    if (select(STDIN_FILENO + 1, &readfds, NULL, NULL, &tv) > 0) {
        return getchar();
    }

    return -1;  // 没有按键被按下
}

int main(int argc, char **argv) 
{
    ros::init(argc, argv, "controller_menu");
    ros::NodeHandle nh;

    std::string packagePath;
    if (!nh.getParam("package_path", packagePath)) {
        ROS_ERROR("Failed to get 'package_path' from the parameter server.");
        return 1;
    }
    std::cout << "package_path: " << packagePath << std::endl;
    int chdir_flags = chdir(packagePath.c_str());
    if (chdir_flags != 0) {
        perror("Error changing directory");  
        return 1;  
    }

    std::string topic_command_sub;
    std::string topic_command_pub;
    nh.param("calibration_command_pub_topic", topic_command_sub, std::string("/livox_hikcamera_cal/command_cal_node"));
    nh.param("calibration_command_sub_topic", topic_command_pub, std::string("/livox_hikcamera_cal/command_controller"));

    std::string pointset_csv_path;
    std::string error_ayalysis_csv_path;
    nh.param("pointset_save_path", pointset_csv_path, std::string("src/livox_hikcamera_cal/data/point_set.csv"));
    nh.param("error_analysis_save_path", error_ayalysis_csv_path, std::string("src/livox_hikcamera_cal/data/border_error_anaylysis.csv"));

    CommandHandler command_handler(nh, topic_command_sub, topic_command_pub);
    PointSetCsvOperator pointset_csv_operator(pointset_csv_path);
    BorderSetCsvOperator borderset_csv_operator(error_ayalysis_csv_path);

    char key;
    enableRawMode();

    while (ros::ok()) {

        int cls_status = system("clear");
        printf("\n--- Main Menu ---\n");
        printf("1. Capture Caliboard\n");
        printf("2. Capture Border Points");
        printf("3. View Points Set\n");
        printf("4. View Border Set\n");
        printf("5. Delete Specific Points Set\n");
        printf("6. Delete Specific Border Set\n");
        printf("7. Delete All Points Sets\n");
        printf("8. Delete All Borders Sets\n");
        printf("9. Exit\n");
        printf("Select an option: ");
        
        key = getchar();  // Read character in raw mode

        if (key == '1') {

            std::vector<geometry_msgs::Point32> group1, group2;
            int local_key;
            while (true) {
                int cls_status = system("clear");  // Use system("cls") on Windows
                pointset_csv_operator.readPointsFromCSV(group1, group2);

                printf("--- Capture Points ---\n");
                for (size_t i = 0; i < group1.size(); ++i) {
                    printf("Set %zu: (%f, %f, %f), (%f, %f, %f)\n",
                        i + 1, group1[i].x, group1[i].y, group1[i].z,
                        group2[i].x, group2[i].y, group2[i].z);
                }

                printf("\nPress Enter to capture new points, Backspace to delete the latest set, or Esc to return to main menu.\n");
                
                local_key = getch(); // Read the key in raw mode

                if (local_key == KEY_ENTER) {  // Enter key

                    command_handler.sendCommand("capture");

                    printf("Capturing... Waiting For Response... Press 'ESC' to abort the capture operation. \n");
                    int abort_key = nonBlockingGetch();  // 初始化并读取第一次按键
                    while(command_handler.getCommand() != "capture_complete") {
                        if (abort_key == KEY_ESC) {
                            break; // 如果按下 ESC 键，则立即退出循环
                        }

                        ros::spinOnce();  // 处理 ROS 消息

                        if (command_handler.getCommand() == "capture_complete") {
                            break; // 如果捕获完成，也退出循环
                        }

                        abort_key = nonBlockingGetch(); // 在循环中不断更新按键状态
                    }

                    command_handler.resetReceivedStatus();

                    printf("Capture Success!\n");
 
                    while(local_key != KEY_ENTER);

                } else if (local_key == KEY_BACKSPACE) {  // Backspace key

                    command_handler.sendCommand("undo");

                    if (!group1.empty()) {
                        pointset_csv_operator.deleteRowFromCSV(group1.size()-0);
                        // pointset_csv_operator.deleteRowFromCSV(group1.size()-1);
                        // pointset_csv_operator.deleteRowFromCSV(group1.size()-2);
                        // pointset_csv_operator.deleteRowFromCSV(group1.size()-3);
                    }
                    int cls_status = system("clear");
                    group1.clear();
                    group2.clear();

                    while(local_key != KEY_BACKSPACE);

                } else if (local_key == KEY_ESC) {  // Esc key
                    break; // Exit the loop and return to the main menu
                }
            }

        } else if (key == '2') {

            pcl::PointCloud<pcl::PointXYZI>::Ptr group1(new pcl::PointCloud<pcl::PointXYZI>);
            std::vector<std::vector<geometry_msgs::Point32>> group2;
            int local_key;
            while (true) {
                int cls_status = system("clear");  // Use system("cls") on Windows
                borderset_csv_operator.readPointsFromCSV(group1, group2);

                printf("--- Capture Borders ---\n");
                for (size_t i = 0; i < group1->size(); ++i) {
                    printf("Set %zu: (%f, %f, %f) || (%f, %f, %f)-(%f, %f, %f)-(%f, %f, %f)-(%f, %f, %f)\n",
                        i + 1, group1->points[i].x, group1->points[i].y, group1->points[i].z,
                        group2[i][0].x, group2[i][0].y, group2[i][0].z,
                        group2[i][1].x, group2[i][1].y, group2[i][1].z,
                        group2[i][2].x, group2[i][2].y, group2[i][2].z,
                        group2[i][3].x, group2[i][3].y, group2[i][3].z
                        );
                }

                printf("\nPress Enter to capture new border Points, Backspace to delete the latest set, or Esc to return to main menu.\n");
                
                local_key = getch(); // Read the key in raw mode

                if (local_key == KEY_ENTER) {  // Enter key

                    command_handler.sendCommand("capture_border");

                    printf("Capturing... Waiting For Response... Press 'ESC' to abort the capture operation. \n");
                    int abort_key = nonBlockingGetch();  // 初始化并读取第一次按键
                    while(command_handler.getCommand() != "capture_complete") {
                        if (abort_key == KEY_ESC) {
                            break; // 如果按下 ESC 键，则立即退出循环
                        }

                        ros::spinOnce();  // 处理 ROS 消息

                        if (command_handler.getCommand() == "capture_border_complete") {
                            break; // 如果捕获完成，也退出循环
                        }

                        abort_key = nonBlockingGetch(); // 在循环中不断更新按键状态
                    }

                    command_handler.resetReceivedStatus();

                    printf("Capture Success!\n");
 
                    while(local_key != KEY_ENTER);

                } else if (local_key == KEY_BACKSPACE) {  // Backspace key

                    command_handler.sendCommand("undo_capture_border");

                    if (!group1->empty()) {
                        borderset_csv_operator.deleteLastSetFromCSV();
                    }
                    int cls_status = system("clear");
                    group1->clear();
                    group2.clear();

                    while(local_key != KEY_BACKSPACE);

                } else if (local_key == KEY_ESC) {  // Esc key
                    break; // Exit the loop and return to the main menu
                }
            }

        } else if (key == '3') {

            int cls_status = system("clear");  // Clear the screen, use system("cls") on Windows systems
            pcl::PointCloud<pcl::PointXYZI>::Ptr group1(new pcl::PointCloud<pcl::PointXYZI>);
            std::vector<std::vector<geometry_msgs::Point32>> group2;
            borderset_csv_operator.readPointsFromCSV(group1, group2);

            printf("--- View Points ---\n");
            if (group1->empty()) {
                printf("No points have been captured yet.\n");
            } else {
                for (size_t i = 0; i < group1->size(); ++i) {
                    printf("Set %zu: (%f, %f, %f) || (%f, %f, %f)-(%f, %f, %f)-(%f, %f, %f)-(%f, %f, %f)\n",
                        i + 1, group1->points[i].x, group1->points[i].y, group1->points[i].z,
                        group2[i][0].x, group2[i][0].y, group2[i][0].z,
                        group2[i][1].x, group2[i][1].y, group2[i][1].z,
                        group2[i][2].x, group2[i][2].y, group2[i][2].z,
                        group2[i][3].x, group2[i][3].y, group2[i][3].z
                        );
                }
            }
            printf("Press any key to return to the main menu.\n");
            getchar();  // Wait for user input to return to the main menu

        }else if (key == '4') {

            int cls_status = system("clear");  // Clear the screen, use system("cls") on Windows systems
            std::vector<geometry_msgs::Point32> group1, group2;
            pointset_csv_operator.readPointsFromCSV(group1, group2);

            printf("--- View Border Points ---\n");
            if (group1.empty()) {
                printf("No points have been captured yet.\n");
            } else {
                for (size_t i = 0; i < group1.size(); ++i) {
                    printf("Set %zu: (%f, %f, %f), (%f, %f, %f)\n",
                        i + 1, group1[i].x, group1[i].y, group1[i].z,
                        group2[i].x, group2[i].y, group2[i].z);
                }
            }
            printf("Press any key to return to the main menu.\n");
            getchar();  // Wait for user input to return to the main menu

        } else if (key == '5') {
            std::vector<geometry_msgs::Point32> group1, group2;
            int current_selection = 0;

            pointset_csv_operator.readPointsFromCSV(group1, group2);
            if (group1.empty()) {
                printf("No Boarder points to delete.\n");
                continue;  // Continue will work as expected if this block is directly inside the main loop.
            }

            while (true) {
                int cls_status = system("clear");
                printf("--- Delete Specific Border Set ---\n");
                for (size_t i = 0; i < group1.size(); ++i) {
                    if (i == current_selection) {
                        printf("--> ");
                    } else {
                        printf("    ");
                    }
                    printf("Set %zu: (%f, %f, %f), (%f, %f, %f)\n",
                        i + 1, group1[i].x, group1[i].y, group1[i].z,
                        group2[i].x, group2[i].y, group2[i].z);
                }

                int key_press = readKey();  // Changed to int to capture potentially EOF
                if (key_press == '\033') {
                    break;
                } else if (key_press == 'A' && current_selection > 0) {
                    current_selection--;
                } else if (key_press == 'B' && current_selection < group1.size() - 1) {
                    current_selection++;
                } else if (key_press == 'E') {  // Assuming KEY_ENTER is the newline character
                    pointset_csv_operator.deleteRowFromCSV(current_selection + 1);
                    pointset_csv_operator.readPointsFromCSV(group1, group2);
                    // Adjust selection if necessary
                    if (current_selection >= group1.size() && !group2.empty()) {
                        current_selection--;
                    }
                    if (group1.empty()) {  // If no items left, exit the loop
                        printf("All sets have been deleted.\n");
                        break;
                    }
                    command_handler.sendCommand("delete_once");
                }
            }
        } else if (key == '6') {
            std::vector<geometry_msgs::Point32> group1, group2;
            int current_selection = 0;

            pointset_csv_operator.readPointsFromCSV(group1, group2);
            if (group1.empty()) {
                printf("No points to delete.\n");
                continue;  // Continue will work as expected if this block is directly inside the main loop.
            }

            while (true) {
                int cls_status = system("clear");
                printf("--- Delete Specific Set ---\n");
                for (size_t i = 0; i < group1.size(); ++i) {
                    if (i == current_selection) {
                        printf("--> ");
                    } else {
                        printf("    ");
                    }
                    printf("Set %zu: (%f, %f, %f), (%f, %f, %f)\n",
                        i + 1, group1[i].x, group1[i].y, group1[i].z,
                        group2[i].x, group2[i].y, group2[i].z);
                }

                int key_press = readKey();  // Changed to int to capture potentially EOF
                if (key_press == '\033') {
                    break;
                } else if (key_press == 'A' && current_selection > 0) {
                    current_selection--;
                } else if (key_press == 'B' && current_selection < group1.size() - 1) {
                    current_selection++;
                } else if (key_press == 'E') {  // Assuming KEY_ENTER is the newline character
                    pointset_csv_operator.deleteRowFromCSV(current_selection + 1);
                    pointset_csv_operator.readPointsFromCSV(group1, group2);
                    // Adjust selection if necessary
                    if (current_selection >= group1.size() && !group2.empty()) {
                        current_selection--;
                    }
                    if (group1.empty()) {  // If no items left, exit the loop
                        printf("All sets have been deleted.\n");
                        break;
                    }
                    command_handler.sendCommand("delete_once");
                }
            }
        } else if (key == '7') {
            // Delete All Sets
            pointset_csv_operator.writePointsToCSVOverwrite(std::vector<geometry_msgs::Point32>(), std::vector<geometry_msgs::Point32>());

        } else if (key == '8') {
            // Delete All Boarder Sets
            borderset_csv_operator.writePointsToCSVOverwrite(pcl::PointCloud<pcl::PointXYZI>::Ptr(new pcl::PointCloud<pcl::PointXYZI>), std::vector<geometry_msgs::Point32>());

        } else if (key == '9') {
            break;  // Exit
        } else {
            // std::cout << "Invalid option, try again.";
        }
    }

    disableRawMode();
    return 0;
}