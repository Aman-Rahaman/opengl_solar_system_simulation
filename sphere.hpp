

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
        float scaling = 1.0f;

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

        static glm::vec3 earth_P0    ;
        static glm::vec3 earth_P1    ;
        static glm::vec3 earth_P2    ;
        static glm::vec3 earth_P3    ;

        static glm::vec3 moon_P0    ;
        static glm::vec3 moon_P1    ;
        static glm::vec3 moon_P2    ;
        static glm::vec3 moon_P3    ;

        static bool     earth_moon_collision;
        static float    earth_t             ;
        static float    moon_t              ;
        static float    deltaTime           ;

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


        static glm::vec3 bezier(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float t) {
            float u = 1 - t;
            return u*u*u*p0 + 3*u*u*t*p1 + 3*u*t*t*p2 + t*t*t*p3;
        }

        static glm::vec3 bezierDerivative(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float t) {
            float u = 1 - t;
            return
                    (
                        3.0f * u * u * (p1 - p0) +
                        6.0f * u * t * (p2 - p1) +
                        3.0f * t * t * (p3 - p2)
                    );
        }

        void set_tranform_matrix_spline(){
            glm::vec3 position;

            if(type == EARTH){
                earth_t += deltaTime;
                if(earth_t >= 1.0f)
                    earth_t = 1.0f;
                position = bezier(earth_P0, earth_P1, earth_P2, earth_P3, earth_t);
            }
            else if(type == MOON){
                moon_t += deltaTime;
                if(moon_t >= 1.0f)
                    moon_t = 1.0f;
                position = bezier(moon_P0, moon_P1, moon_P2, moon_P3, moon_t);
            }
            
            glm::mat4 transform = glm::mat4(1.0f);
            transform = glm::translate(transform, position);

            if(type == EARTH){
                transform = glm::rotate(transform, earth_rot_angle, glm::vec3(0.0f, 1.0f, 0.0f));
                transform = glm::rotate(transform, 0.41f, glm::vec3(0.0f, 0.0f, 1.0f));
            }
            else if(type == MOON)
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
                x = EARTH_REV_RADIUS * cos(earth_rev_angle);
                z = EARTH_REV_RADIUS * sin(earth_rev_angle);
            }
            else if(type == SUN){
                x = SUN_REV_RADIUS * cos(sun_rev_angle);
                z = SUN_REV_RADIUS * sin(sun_rev_angle);
            }
            else if(type == MOON){
                x = MOON_REV_RADIUS * cos(moon_rev_angle);
                z = MOON_REV_RADIUS * sin(moon_rev_angle);
            }
            else if(type == SPACE){
                x = SPACE_REV_SPEED * cos(space_rev_angle);
                z = SPACE_REV_SPEED * sin(space_rev_angle);
            }
            transform = glm::translate(transform, glm::vec3(x, y, z));

            if(type == MOON){
                x = EARTH_REV_RADIUS * cos(earth_rev_angle);
                z = EARTH_REV_RADIUS * sin(earth_rev_angle);
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


            if(type==SUN && motionType==SPLINE)
                this->scaling += this->deltaTime * 1.5f;
            transform = glm::scale(transform, glm::vec3(this->scaling, this->scaling, this->scaling));

            
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

            earth_rev_angle += EARTH_REV_SPEED;
            earth_rot_angle += EARTH_ROT_SPEED;

            sun_rev_angle += SUN_REV_SPEED;
            sun_rot_angle += SUN_ROT_SPEED;

            moon_rev_angle += MOON_REV_SPEED;
            moon_rot_angle += MOON_ROT_SPEED;

            space_rev_angle += SPACE_REV_SPEED;
            space_rot_angle += SPACE_ROT_SPEED;
            
        }

        void render() {
            bindTexture();

            if(motionType==SPLINE && (type==EARTH || type==MOON)){
                set_tranform_matrix_spline();
                if( earth_moon_collision_check() ){
                    set_new_spline_after_collision();
                }
            }
            else{
                set_tranform_matrix();
            }

            setup_camera();

            setup_light_source();

            draw_shape();
        }

        static void set_new_spline_after_collision(){
            glm::vec3 earth_position    = bezier(earth_P0, earth_P1, earth_P2, earth_P3, earth_t);
            glm::vec3 moon_position     = bezier(moon_P0, moon_P1, moon_P2, moon_P3, moon_t);
            glm::vec3 L_O_C = glm::normalize(moon_position - earth_position);       // L_O_C = line of centre. line connecting the centres of 2 spheres


            glm::vec3 earth_vel = bezierDerivative(earth_P0, earth_P1, earth_P2, earth_P3, earth_t);
            glm::vec3 moon_vel = bezierDerivative(moon_P0, moon_P1, moon_P2, moon_P3, moon_t);


            float earth_mass = 10.0f;  // earth mass
            float moon_mass = 1.0f;  // moon mass


            glm::vec3 relative_velocity = earth_vel - moon_vel;
            float vel_along_L_O_C = glm::dot(relative_velocity, L_O_C);
            float e = 1.0f;  // perfectly elastic
            float j = -(1 + e) * vel_along_L_O_C / (1.0f/earth_mass + 1.0f/moon_mass);
            glm::vec3 impulse = j * L_O_C;

            
            glm::vec3 earth_velocity = earth_vel + impulse / earth_mass;
            glm::vec3 moon_velocity = moon_vel - impulse / moon_mass;

            // ------------------------------------------------------------------------------------------------------------------
            // finding new spline points after collision
            motionType = SPLINE;
            float tangent_distance_earth = 20.0f;
            float tangent_distance_moon = 50.0f;

            earth_P0 = earth_position;
            
            glm::vec3 r = earth_P0;         // Vector from Sun to Earth
            glm::vec3 tangent = glm::normalize(earth_velocity); // Tangent vector in the orbit plane
            earth_P1 = earth_P0 + tangent * tangent_distance_earth;

            r = earth_P1;         
            tangent = glm::normalize(glm::cross(r, glm::vec3(0.0f, 1.0f, 0.0f))); 
            earth_P2 = earth_P1 + tangent * tangent_distance_earth;

            r = earth_P2;         
            tangent = glm::normalize(glm::cross(r, glm::vec3(0.0f, 1.0f, 0.0f)));
            earth_P3 = earth_P2 + tangent * tangent_distance_earth;


            moon_P0 = moon_position;
            
            r = moon_P0;         // Vector from Sun to moon
            tangent = glm::normalize(moon_velocity); // Tangent vector in the orbit plane
            moon_P1 = moon_P0 + tangent * tangent_distance_moon;

            r = moon_P1;         
            tangent = glm::normalize(glm::cross(r, glm::vec3(0.0f, 1.0f, 0.0f))); 
            moon_P2 = moon_P1 + tangent * tangent_distance_moon;

            r = earth_P2;         
            tangent = glm::normalize(glm::cross(r, glm::vec3(0.0f, 1.0f, 0.0f)));
            moon_P3 = moon_P2 + tangent * tangent_distance_moon;

            // ------------------------------------------------------------------------------------------------------------------
            // reset the parameter 't' for the spline
            earth_t = 0.0f;
            moon_t = 0.0f;
        }
        
        static void set_spline(){
            motionType = SPLINE;
            float tangent_distance_earth = 20.0f;
            float tangent_distance_moon = 15.0f;



            float x = EARTH_REV_RADIUS * cos(earth_rev_angle);
            float z = EARTH_REV_RADIUS * sin(earth_rev_angle);
            float y = 0.0f;

            glm::vec3 earth_position(x, y, z);
            earth_P0 = earth_position;
            
            glm::vec3 r = earth_P0;         // Vector from Sun to Earth
            glm::vec3 tangent = glm::normalize(glm::cross(r, glm::vec3(0.0f, 1.0f, 0.0f))); // Tangent vector in the orbit plane
            earth_P1 = earth_P0 + tangent * tangent_distance_earth;

            r = earth_P1;         
            tangent = glm::normalize(glm::cross(r, glm::vec3(0.0f, 1.0f, 0.0f))); 
            earth_P2 = earth_P1 + tangent * tangent_distance_earth;

            r = earth_P2;         
            tangent = glm::normalize(glm::cross(r, glm::vec3(0.0f, 1.0f, 0.0f)));
            earth_P3 = earth_P2 + tangent * tangent_distance_earth;



            x = MOON_REV_RADIUS * cos(moon_rev_angle);
            z = MOON_REV_RADIUS * sin(moon_rev_angle);
            y = 0.0f;

            glm::vec3 moon_position(x, y, z);
            moon_position = moon_position + earth_position;
            moon_P0 = moon_position;
            
            r = moon_P0;         // Vector from Sun to moon
            tangent = glm::normalize(glm::cross(r, glm::vec3(0.0f, 1.0f, 0.0f))); // Tangent vector in the orbit plane
            moon_P1 = moon_P0 + tangent * tangent_distance_moon;

            r = moon_P1;         
            tangent = glm::normalize(glm::cross(r, glm::vec3(0.0f, 1.0f, 0.0f))); 
            moon_P2 = moon_P1 + tangent * tangent_distance_moon;

            r = earth_P2;         
            tangent = glm::normalize(glm::cross(r, glm::vec3(0.0f, 1.0f, 0.0f)));
            moon_P3 = moon_P2 + tangent * tangent_distance_moon;

        }

        static bool earth_moon_collision_check(){
            if(earth_moon_collision) return false;

            glm::vec3 earth_position    = bezier(earth_P0, earth_P1, earth_P2, earth_P3, earth_t);
            glm::vec3 moon_position     = bezier(moon_P0, moon_P1, moon_P2, moon_P3, moon_t);
            
            float distance = glm::length(earth_position - moon_position);
            if (distance <= (0.5f * (EARTH_RADIUS + MOON_RADIUS)) + 0.001f) {              // this 0.5f is coming from vertices variable in the important_variables.hpp
                // Collision detected!
                cout << "collided" << endl;
                earth_moon_collision = true;
                return true;
            }
            return false;
        }




};





#endif

