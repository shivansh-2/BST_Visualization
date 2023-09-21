#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <GL/glut.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define MAX_INPUT_LENGTH 10

int deleteButtonActive = 0;


typedef struct Node {
    int key;
    struct Node* left;
    struct Node* right;
} Node;



Node* root = NULL;
char inputBuffer[MAX_INPUT_LENGTH];
int inputIndex = 0;
int displayInput = 0;

int textAreaActive = 0;
char textAreaInput[MAX_INPUT_LENGTH];
int textAreaIndex = 0;

Node* minValueNode(Node* node) {
    Node* current = node;
    while (current && current->left != NULL)
        current = current->left;
    return current;
}



Node* createNode(int key) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->key = key;
    newNode->left = newNode->right = NULL;
    return newNode;
}

Node* deleteNode(Node* root, int key) {
    if (root == NULL)
        return root;

    if (key < root->key)
        root->left = deleteNode(root->left, key);
    else if (key > root->key)
        root->right = deleteNode(root->right, key);
    else {
        if (root->left == NULL) {
            Node* temp = root->right;
            free(root);
            return temp;
        } else if (root->right == NULL) {
            Node* temp = root->left;
            free(root);
            return temp;
        }

        Node* temp = minValueNode(root->right);
        root->key = temp->key;
        root->right = deleteNode(root->right, temp->key);
    }
    return root;
}


Node* insertNode(Node* root, int key) {
    if (root == NULL)
        return createNode(key);

    if (key < root->key)
        root->left = insertNode(root->left, key);
    else if (key > root->key)
        root->right = insertNode(root->right, key);

    return root;
}

void drawNode(Node* node, int x, int y) {
    char buffer[10];
    sprintf(buffer, "%d", node->key);

    // Set node color to light blue
    glColor3f(0.6, 0.8, 1.0);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i++) {
        float theta = i * 3.14159 / 180.0;
        glVertex2f(x + 19 * cos(theta), y + 19 * sin(theta));
    }
    glEnd();

    // Set text color to black
    glColor3f(0.0, 0.0, 0.0);
    glRasterPos2f(x - 5, y - 5);
    for (int i = 0; buffer[i] != '\0'; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, buffer[i]);
    }
}

void drawTree(Node* root, int x, int y, int level, int dx, int spacing) {
    if (root == NULL)
        return;

    if (root->left != NULL) {
        // Set line color to light green
        glColor3f(0.0, 0.0, 0.0);
        glBegin(GL_LINES);
        glVertex2f(x, y);
        glVertex2f(x - dx, y - spacing);
        glEnd();
        drawTree(root->left, x - dx, y - spacing, level - 1, dx / 2, spacing);
    }

    if (root->right != NULL) {
        // Set line color to light green
        glColor3f(0.0, 0.0, 0.0);
        glBegin(GL_LINES);
        glVertex2f(x, y);
        glVertex2f(x + dx, y - spacing);
        glEnd();
        drawTree(root->right, x + dx, y - spacing, level - 1, dx / 2, spacing);
    }

    drawNode(root, x, y);
}

void drawButton(int x, int y, int width, int height, const char* label) {
    // Set button color to light gray
    glColor3f(0.8, 0.8, 0.8);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();

    // Set text color to black
    glColor3f(0.0, 0.0, 0.0);
    glRasterPos2f(x + 5, y + 5);
    for (int i = 0; label[i] != '\0'; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, label[i]);
    }
}

void drawTextArea(int x, int y, int width, int height) {
    // Set text area border color to white
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();

    // Set text area fill color to light gray
    glColor3f(0.8, 0.8, 0.8);
    glBegin(GL_QUADS);
    glVertex2f(x + 1, y + 1);
    glVertex2f(x + width - 1, y + 1);
    glVertex2f(x + width - 1, y + height - 1);
    glVertex2f(x + 1, y + height - 1);
    glEnd();

    // Set text color to black
    glColor3f(0.0, 0.0, 0.0);
    glRasterPos2f(x + 5, y + height / 2 - 5);
    for (int i = 0; textAreaInput[i] != '\0'; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, textAreaInput[i]);
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    int level = 8;
    int dx = 100; 
    int spacing = 70;
    int rootX = glutGet(GLUT_WINDOW_WIDTH) / 2;  // X position of the root node
    int rootY = glutGet(GLUT_WINDOW_HEIGHT) - spacing;  // Y position of the root node

    if (root != NULL) {
        drawTree(root, rootX, rootY, level, dx, spacing);
    }

    if (displayInput) {
        drawTextArea(10, 20, WINDOW_WIDTH - 20, 20);
    } else {
        drawButton(10, 20, 100, 20, "Add Node");
        if (!displayInput && !deleteButtonActive) {
            //drawDeleteButton(120, 20, 100, 20, "Delete Node");
            drawButton(120, 20, 100, 20, "Delete Node");
        }
    }

    glFlush();
}


void reshape(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, width, 0, height);
    glMatrixMode(GL_MODELVIEW);
}

void mouse(int button, int state, int x, int y) {
    int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
    int buttonY = windowHeight - 40;
    int buttonHeight = 20;

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        if (!displayInput && x >= 10 && x <= 110 && y >= buttonY && y <= buttonY + buttonHeight) {
            displayInput = 1;
            glutPostRedisplay();
        } else if (!displayInput && x >= 120 && x <= 220 && y >= buttonY && y <= buttonY + buttonHeight) {
            displayInput = 2;  // Indicate delete mode
            memset(textAreaInput, 0, sizeof(textAreaInput));
            textAreaIndex = strlen(textAreaInput);
            glutPostRedisplay();
        }
    }
}




void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 27:  // ESC key
            exit(0);
            break;
    }
}

void processNormalKeys(unsigned char key, int x, int y) {
    if (displayInput == 1) {  // Insert mode
        if (key == '\r' || key == '\n') {
            displayInput = 0;
            if (textAreaIndex > 0) {
                textAreaInput[textAreaIndex] = '\0';
                int nodeKey = atoi(textAreaInput);
                root = insertNode(root, nodeKey);
                textAreaIndex = 0;
                memset(textAreaInput, 0, sizeof(textAreaInput));
                glutPostRedisplay();
            }
        } else if (key == 8) {  // Backspace key
            if (textAreaIndex > 0) {
                textAreaIndex--;
                textAreaInput[textAreaIndex] = '\0';
                glutPostRedisplay();
            }
        } else if (textAreaIndex < MAX_INPUT_LENGTH) {
            textAreaInput[textAreaIndex++] = key;
            glutPostRedisplay();
        }
    } else if (displayInput == 2) {  // Delete mode
        if (key == '\r' || key == '\n') {
            displayInput = 0;
            if (textAreaIndex > 0) {
                textAreaInput[textAreaIndex] = '\0';
                int nodeKey = atoi(textAreaInput);
                root = deleteNode(root, nodeKey);
                textAreaIndex = 0;
                memset(textAreaInput, 0, sizeof(textAreaInput));
                glutPostRedisplay();
            }
        } else if (key == 8) {  // Backspace key
            if (textAreaIndex > 0) {
                textAreaIndex--;
                textAreaInput[textAreaIndex] = '\0';
                glutPostRedisplay();
            }
        } else if (textAreaIndex < MAX_INPUT_LENGTH) {
            textAreaInput[textAreaIndex++] = key;
            glutPostRedisplay();
        }
    }
}



int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Binary Search Tree Visualization");
    glClearColor(1.0, 1.0, 1.0, 0.0);
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutPassiveMotionFunc(NULL);
    glutMotionFunc(NULL);
    glutMouseFunc(mouse);
    glutKeyboardUpFunc(processNormalKeys);

    glutMainLoop();
    return 0;
}

