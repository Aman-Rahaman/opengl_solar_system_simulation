#ifndef DATA_TYPES_HPP
#define DATA_TYPES_HPP


#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
using namespace std;

#include "./config.hpp"


enum CelestialObject {
    EARTH,
    SUN,
    MOON,
    SPACE
  };


enum MotionType { ORBIT, SPLINE };


class Sphere {
    private:
        static MotionType motionType;
        float deltaTime = 0.001f;
        float t = 0.0f;

        float size = 1.0f;
        vector<float> verticesInfo;
        GLuint VBO, VAO, EBO;
        int totalNumberOfPoints = 0;
        GLuint texture;
        enum CelestialObject type;
        int resolution_level;

        static float earth_rev_angle    ;
        static float sun_rev_angle      ;
        static float moon_rev_angle     ;
        static float space_rev_angle    ;

        static float earth_rot_angle    ;
        static float sun_rot_angle      ;
        static float moon_rot_angle     ;
        static float space_rot_angle    ;

        static float earth_rev_radius   ;
        static float sun_rev_radius     ;
        static float moon_rev_radius    ;
        static float space_rev_radius   ;
        
        static float earth_rev_speed    ;
        static float sun_rev_speed      ;
        static float moon_rev_speed     ;
        static float space_rev_speed    ;
        
        static float earth_rot_speed    ;
        static float sun_rot_speed      ;
        static float moon_rot_speed     ;
        static float space_rot_speed    ;

        static glm::vec3 earth_P0    ;
        static glm::vec3 earth_P1    ;
        static glm::vec3 earth_P2    ;
        static glm::vec3 earth_P3    ;


        float calculateColor(float c){
            return (abs(c) + 0.5f);
        }
        
        
        void showVertices(){
            cout << endl;
            for(int i=0; i<this->verticesInfo.size(); i++){
                if(i%6==0) cout << endl;
                cout << this->verticesInfo[i] << ", ";
            }
            cout << endl;
        }

        void normalizeAndInsertVertex(float x, float y, float z){
            float x2, y2, z2;
            float d = pow(x*x + y*y + z*z, 0.5);
            x2 = 0.5f * x / d;
            y2 = 0.5f * y / d;
            z2 = 0.5f * z / d;

            // coordinates
            this->verticesInfo.push_back(x2);
            this->verticesInfo.push_back(y2);
            this->verticesInfo.push_back(z2);

            // normal ( the normal will same as the coordinates because the object is a sphere and the centre is the origin )
            this->verticesInfo.push_back(x2);
            this->verticesInfo.push_back(y2);
            this->verticesInfo.push_back(z2);

            // texture coordinates
            float u = atan2(z2, x2) / (2.0f * M_PI) + 0.5f;
            float v = y2 + 0.5f;  // Corrected v calculation
            this->verticesInfo.push_back(u);
            this->verticesInfo.push_back(v);
        }

        void divideAndInsertTrianlges(int level, 
            float x1, float y1, float z1,
            float x2, float y2, float z2,
            float x3, float y3, float z3
        ){
            
            if(level >= this->resolution_level){
                normalizeAndInsertVertex(x1, y1, z1);
                normalizeAndInsertVertex(x2, y2, z2);
                normalizeAndInsertVertex(x3, y3, z3);
                return;
            }

            float x4, y4, z4,   x5, y5, z5,   x6, y6, z6;

            x4=(x1+x2)/2; y4=(y1+y2)/2; z4=(z1+z2)/2;
            x5=(x2+x3)/2; y5=(y2+y3)/2; z5=(z2+z3)/2;
            x6=(x3+x1)/2; y6=(y3+y1)/2; z6=(z3+z1)/2;

            divideAndInsertTrianlges(level+1, 
                x1, y1, z1,
                x4, y4, z4,
                x6, y6, z6
            );

            divideAndInsertTrianlges(level+1, 
                x2, y2, z2,
                x4, y4, z4,
                x5, y5, z5
            );

            divideAndInsertTrianlges(level+1, 
                x3, y3, z3,
                x5, y5, z5,
                x6, y6, z6
            );

            divideAndInsertTrianlges(level+1, 
                x5, y5, z5,
                x4, y4, z4,
                x6, y6, z6
            );

        }


        void generateTriangles(){
            for(int i=0; i<9*8; i+=9){
                divideAndInsertTrianlges(0, 
                    vertices[i], vertices[i+1], vertices[i+2],
                    vertices[i+3], vertices[i+4], vertices[i+5],
                    vertices[i+6], vertices[i+7], vertices[i+8]
                );
            }
            
            this->totalNumberOfPoints = this->verticesInfo.size() / 6;
        }

        void setupBuffer(){
            // Set up vertex data and buffers
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);

            glBindVertexArray(VAO);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, this->verticesInfo.size() * sizeof(float), this->verticesInfo.data(), GL_STATIC_DRAW);

            // Position attribute
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            // Normal attribute
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);

            // texture coordinate attribute
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); // Texture Coord
            glEnableVertexAttribArray(2);
        }

        void addTexture(){

            const char* texture_file_name = nullptr;
            if(this->type == EARTH)
                texture_file_name = "earth.jpg";
            else if(this->type == SUN)
                texture_file_name = "sun.jpg";
            else if(this->type == MOON)
                texture_file_name = "moon.jpg";
            else if(this->type == SPACE)
                texture_file_name = "stars.jpg";

            glGenTextures(1, &this->texture);
            glBindTexture(GL_TEXTURE_2D, this->texture);

            // Set texture wrapping and filtering
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            // Load image
            int width, height, nrChannels;
            unsigned char *data = stbi_load(texture_file_name, &width, &height, &nrChannels, 0);
            if (data) {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_2D);
            } else {
                std::cerr << "Failed to load texture" << std::endl;
            }
            stbi_image_free(data);
        }

        void bindTexture(){
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, this->texture);
            glUniform1i(glGetUniformLocation(shaderProgram, "Texture"), 0);
        }

        void setup_light_source(){
            // Send light uniforms to the shader
            int lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos");
            glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));

            int lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");
            glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

            int viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos");
            glUniform3fv(viewPosLoc, 1, glm::value_ptr(cameraPos));
        }

        void setup_camera(){
            // camera view
            glm::mat4 rotation;
            rotation = glm::rotate(glm::mat4(1.0f), cameraAngleY,   glm::vec3(0.0f, 1.0f, 0.0f));
            rotation = glm::rotate(rotation, cameraAngleX,   glm::vec3(1.0f, 0.0f, 0.0f));
            rotation = glm::rotate(rotation,  cameraAngleZ,  glm::vec3(0.0f, 0.0f, 1.0f));

            glm::vec3 forward = glm::vec3(rotation * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));
            up = glm::vec3(rotation * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
            cameraPos = target - forward * cameraRadius;

            glm::mat4 view = glm::lookAt(cameraPos, target, up);
            int viewLoc = glGetUniformLocation(shaderProgram, "view");
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));


            glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
            int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
            glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        }

        void draw_shape(){
            glBindVertexArray(this->VAO);
            glDrawArrays(GL_TRIANGLES, 0, this->totalNumberOfPoints);
            glBindVertexArray(0);
        }

        void reverse_normals(){
            for(int i=0; i<this->verticesInfo.size(); i+=8){
                this->verticesInfo[i + 3]   *= -1;
                this->verticesInfo[i + 4]   *= -1;
                this->verticesInfo[i + 5]   *= -1;
            }
        }

        void fix_size(){
            for(int i=0; i<this->verticesInfo.size(); i+=8){
                this->verticesInfo[i]       *= this->size;
                this->verticesInfo[i + 1]   *= this->size;
                this->verticesInfo[i + 2]   *= this->size;
            }
        }


        glm::vec3 bezier(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float t) {
            float u = 1 - t;
            return u*u*u*p0 + 3*u*u*t*p1 + 3*u*t*t*p2 + t*t*t*p3;
        }

        void set_tranform_matrix_spline(){
            this->t += this->deltaTime;
            if(this->t >= 1.0f)
                this->t = 1.0f;
            
            glm::vec3 position = bezier(earth_P0, earth_P1, earth_P2, earth_P3, t);
            
            glm::mat4 transform = glm::mat4(1.0f);
            transform = glm::translate(transform, position);

            if(type == EARTH){
                transform = glm::rotate(transform, earth_rot_angle, glm::vec3(0.0f, 1.0f, 0.0f));
                transform = glm::rotate(transform, 0.41f, glm::vec3(0.0f, 0.0f, 1.0f));
            }
            if(type == MOON)
                transform = glm::rotate(transform, moon_rot_angle, glm::vec3(0.0f, 1.0f, 0.0f));

            int transformLoc = glGetUniformLocation(shaderProgram, "transform");
            glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
            
        }

        void set_tranform_matrix(){
            glm::mat4 transform = glm::mat4(1.0f);

            // Compute revolution position
            float x = 0;
            float z = 0;
            float y = 0; // Keep it at the same height

            if(type == EARTH){
                x = earth_rev_radius * cos(earth_rev_angle);
                z = earth_rev_radius * sin(earth_rev_angle);
            }
            else if(type == SUN){
                x = sun_rev_radius * cos(sun_rev_angle);
                z = sun_rev_radius * sin(sun_rev_angle);
            }
            else if(type == MOON){
                x = moon_rev_radius * cos(moon_rev_angle);
                z = moon_rev_radius * sin(moon_rev_angle);
            }
            else if(type == SPACE){
                x = space_rev_speed * cos(space_rev_angle);
                z = space_rev_speed * sin(space_rev_angle);
            }
            transform = glm::translate(transform, glm::vec3(x, y, z));

            if(type == MOON){
                x = earth_rev_radius * cos(earth_rev_angle);
                z = earth_rev_radius * sin(earth_rev_angle);
                transform = glm::translate(transform, glm::vec3(x, y, z));
            }


            // for rotation about its axis
            if(type == EARTH)
                transform = glm::rotate(transform, earth_rot_angle, glm::vec3(0.0f, 1.0f, 0.0f));
            if(type == SUN)
                transform = glm::rotate(transform, sun_rot_angle, glm::vec3(0.0f, 1.0f, 0.0f));
            if(type == MOON)
                transform = glm::rotate(transform, moon_rot_angle, glm::vec3(0.0f, 1.0f, 0.0f));
            if(type == SPACE)
                transform = glm::rotate(transform, space_rot_angle, glm::vec3(0.0f, 1.0f, 0.0f));
    

            // for earth tilted axis
            if(type == EARTH) 
                transform = glm::rotate(transform, 0.41f, glm::vec3(0.0f, 0.0f, 1.0f));

            
            int transformLoc = glGetUniformLocation(shaderProgram, "transform");
            glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
        }


    public:
        // Constructor
        Sphere(float sphere_size, enum CelestialObject type, int res_level) {
            this->size = sphere_size;
            this->type = type;
            this->resolution_level = res_level;

            generateTriangles();
            fix_size();
            if(type == SUN || type == SPACE) 
                reverse_normals();

            setupBuffer();
            addTexture();
        }

        static void initialize_config_variables();

        void clean(){
            glDeleteVertexArrays(1, &this->VAO);
            glDeleteBuffers(1, &this->VBO);
            glDeleteBuffers(1, &this->EBO);
        }

        static void update_angles() {

            earth_rev_angle += earth_rev_speed;
            earth_rot_angle += earth_rot_speed;

            sun_rev_angle += sun_rev_speed;
            sun_rot_angle += sun_rot_speed;

            moon_rev_angle += moon_rev_speed;
            moon_rot_angle += moon_rot_speed;

            space_rev_angle += space_rev_speed;
            space_rot_angle += space_rot_speed;
            
        }

        void render() {
            bindTexture();

            if(motionType==SPLINE && type==EARTH){
                set_tranform_matrix_spline();
            }
            else{
                set_tranform_matrix();
            }

            setup_camera();

            setup_light_source();

            draw_shape();
        }
        
        static void set_spline(){
            motionType = SPLINE;

            float x = earth_rev_radius * cos(earth_rev_angle);
            float z = earth_rev_radius * sin(earth_rev_angle);
            float y = 0.0f;

            glm::vec3 earth_posistion(x, y, z);

            float tangent_distance = 12.0f;

            earth_P0 = earth_posistion;
            
            glm::vec3 r = earth_P0;         // Vector from Sun to Earth
            glm::vec3 tangent = glm::normalize(glm::cross(r, glm::vec3(0.0f, 1.0f, 0.0f))); // Tangent vector in the orbit plane
            earth_P1 = earth_P0 + tangent * tangent_distance;

            r = earth_P1;         // Vector from Sun to Earth
            tangent = glm::normalize(glm::cross(r, glm::vec3(0.0f, 1.0f, 0.0f))); // Tangent vector in the orbit plane
            earth_P2 = earth_P1 + tangent * tangent_distance;

            r = earth_P2;         // Vector from Sun to Earth
            tangent = glm::normalize(glm::cross(r, glm::vec3(0.0f, 1.0f, 0.0f))); // Tangent vector in the orbit plane
            earth_P3 = earth_P2 + tangent * tangent_distance;
        }
};





#endif

