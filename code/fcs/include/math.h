#pragma once

typedef struct Vector3 {
    float x;
    float y;
    float z;
} Vector3;

Vector3 vector3add(Vector3 vec, Vector3 vec1) {
    vec.x += vec1.x;
    vec.y += vec1.y;
    vec.z += vec1.z;
    return vec;
}

Vector3 vector3multiply(Vector3 vec, Vector3 vec1) {
    vec.x *= vec1.x;
    vec.y *= vec1.y;
    vec.z *= vec1.z;
    return vec;
}

Vector3 vector3divide(Vector3 vec, Vector3 vec1) {
    vec.x /= vec1.x;
    vec.y /= vec1.y;
    vec.z /= vec1.z;
    return vec;
}

Vector3 vector3scale(Vector3 vec, float scale) {
    vec.x *= scale;
    vec.y *= scale;
    vec.z *= scale;
    return vec;
}

Vector3 vector3shorten(Vector3 vec, float scale) {
    vec.x /= scale;
    vec.y /= scale;
    vec.z /= scale;
    return vec;
}

