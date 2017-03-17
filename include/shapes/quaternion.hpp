#pragma once

#include <cmath>
#include "shapes\vector3f.hpp"
#include "defines.hpp"

#define Q_DEGREES true
#define Q_RADIANS false

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#define CLAMP(x, upper, lower) (MIN(upper, MAX(x, lower)))

class Quaternion {
public:
	float x, y, z, w;

	Quaternion() {
		x = 0;
		y = 0;
		z = 0;
		w = 0;
	}

	Quaternion(Vector3f euler, bool degrees = false) {
		//Convert a Vector3f containing an Euler angle rotation to a unit quaternion
		float cr, cp, cy, sr, sp, sy, cpcy, spsy;

		if (degrees) {
			cr = std::cos(D2R(euler.z) / 2);
			cp = std::cos(D2R(euler.y) / 2);
			cy = std::cos(D2R(euler.x) / 2);

			sr = std::sin(D2R(euler.z) / 2);
			sp = std::sin(D2R(euler.y) / 2);
			sy = std::sin(D2R(euler.x) / 2);
		}
		else
		{
			cr = std::cos(euler.z / 2);
			cp = std::cos(euler.y / 2);
			cy = std::cos(euler.x / 2);

			sr = std::sin(euler.z / 2);
			sp = std::sin(euler.y / 2);
			sy = std::sin(euler.x / 2);
		}

		cpcy = cp*cy;
		spsy = sp*sy;

		w = cr*cp*cy + sr*sp*sy;
		x = sr*cp*cy - cr*sp*sy;
		y = cr*sp*cy + sr*cp*sy;
		z = cr*cp*sy - sr*sp*cy;
	}

	Quaternion(float real, float _x, float _y, float _z) : w(real), x(_x), y(_y), z(_z) {}
	Quaternion(float real, const Vector3f &i) : w(real), x(i.x), y(i.y), z(i.z) {}

	static Quaternion fromAxisAngle(float angle, Vector3f axis){
		float half_sin = std::sin(0.5f / angle);
		float half_cos = std::cos(0.5f * angle);

		return Quaternion(half_cos, half_sin*axis.x, half_sin*axis.y, half_sin*axis.z);
	}

	static Quaternion fromTwoVectors(Vector3f v1, Vector3f v2){
		float cos_theta = Vector3f::dot_product(v1.normalize(), v2.normalize());
		float angle = std::acos(cos_theta);
		Vector3f v3 = Vector3f::cross_product(v1, v2).normalize();
		return fromAxisAngle(angle, v3);
	}

	Vector3f toVector3f(bool degrees, bool homogenous = true) {
		float w_sq = w*w;
		float x_sq = x*x;
		float y_sq = y*y;
		float z_sq = z*z;

		Vector3f euler;

        if(degrees){
            if (homogenous) {
                euler.x = R2D(std::atan2(2.0f * (x*y + z*w), x_sq - y_sq - z_sq + w_sq));
                euler.y = R2D(std::asin(-2.0f * (x*z - y*w)));
                euler.z = R2D(std::atan2(2.0f * (y*z + x*w), -x_sq - y_sq + z_sq + w_sq));
            }
            else
            {
                euler.x = R2D(std::atan2(2.0f * (x*y + z*w), 1 - 2 * (x_sq + y_sq)));
                euler.y = R2D(std::asin(-2.0f * (x*z - y*w)));
                euler.z = R2D(std::atan2(2.0f * (y*z + x*w), 1 - 2 * (y_sq + z_sq)));
            }
        } 
        else {
            if (homogenous) {
                euler.x = (std::atan2(2.0f * (x*y + z*w), x_sq - y_sq - z_sq + w_sq));
                euler.y = (std::asin(-2.0f * (x*z - y*w)));
                euler.z = (std::atan2(2.0f * (y*z + x*w), -x_sq - y_sq + z_sq + w_sq));
            }
            else
            {
                euler.x = (std::atan2(2.0f * (x*y + z*w), 1 - 2 * (x_sq + y_sq)));
                euler.y = (std::asin(-2.0f * (x*z - y*w)));
                euler.z = (std::atan2(2.0f * (y*z + x*w), 1 - 2 * (y_sq + z_sq)));
            }
        }
		

		return euler;
	}

	Quaternion operator*(const Quaternion& q2) {
		Quaternion result;

		result.x = this->x * q2.w + this->y * q2.z - this->z * q2.y + this->w * q2.x;
		result.y = -this->x * q2.z + this->y * q2.w + this->z * q2.x + this->w * q2.y;
		result.z = this->x * q2.y - this->y * q2.x + this->z * q2.w + this->w * q2.z;
		result.w = -this->x * q2.x - this->y * q2.y - this->z * q2.z + this->w * q2.w;

		return result;	
	}

	Quaternion operator+(const Quaternion& q2) {
		Quaternion result;

		result.x = x + q2.x;
		result.y = y + q2.y;
		result.z = z + q2.z;
		result.w = w + q2.w;

		return result;
	}

	Quaternion operator- (const Quaternion& q2) {
		return Quaternion(w - q2.w, Vector3f(x, y, z) - Vector3f(q2.x, q2.y, q2.z));
	}

	Quaternion &operator -=(const Quaternion &q2) {
		x = -q2.x;
		y = -q2.y;
		z = -q2.z;
		w = -q2.w;

		return *this;
	}

	Quaternion operator* (float scale) {
		return Quaternion(w*scale, Vector3f(x, y, z)*scale);
	}

	Quaternion operator/ (float scale) {
		return Quaternion(w / scale, Vector3f(x, y, z) / scale);
	}

	Quaternion operator -() {
		return Quaternion(-w, -x, -y, -z);
	}

	static float dot_product(const Quaternion& q1, const Quaternion& q2) {
		return Vector3f(q1.x, q1.y, q1.z) * Vector3f(q2.x, q2.y, q2.z) + q1.w*q2.w;
	}

	float length() {
		return (float)std::sqrt(w*w + Vector3f(x, y, z)*Vector3f(x, y, z));
	}

	Quaternion normalized() {
		return *this / length();
	}

	Quaternion lerp(Quaternion& q2, float t) {
		return (*this * (1 - t) + q2*t).normalized();
	}

	Quaternion slerp(const Quaternion& q2, float t) {
		Quaternion q3;

		float dot = Quaternion::dot_product(*this, q2);

		if (dot < 0) {
			dot = -dot;
			q3 = Quaternion(-q2.w, -q2.x, -q2.y, -q2.z);
		}
		else {
			q3 = q2;
		}

		if (dot < 0.95f) {
			float angle = std::acos(dot);
			return (*this * std::sin(angle*(1 - t)) + q3 * std::sin(angle*t)) / std::sin(angle);
		}
		else {
			return lerp(q3, t);
		}
	}

	Quaternion rotate_toward(Quaternion dest, float maxAngle)
	{
		if(maxAngle < 0.001f){
			return *this; //no rotation.
		}

		float cosTheta = dot_product(*this, dest);

		if(cosTheta > 0.999f){
			//they're ~equal, pretend we're already at dest.
			return dest;
		}

		if(cosTheta < 0){
			//Reverse so we take a short path around the sphere
			*this = *this * -1.0;
			cosTheta *= 1.0f;
		}

		float angle = std::acos(cosTheta);

		if(angle < maxAngle){
			//We need to move fewer degrees than allowed, return destination
			return dest;
		}

		float t = maxAngle / angle;
		angle = maxAngle;

		Quaternion result = ((*this) * (std::sin(1 - t) * angle)) + (dest * std::sin(t * angle)) / std::sin(angle);
		return result.normalized();
	}
	
	static Quaternion RotationBetweenVectors(Vector3f start, Vector3f end);
};

Quaternion Quaternion::RotationBetweenVectors(Vector3f start, Vector3f end) {
	start = start.normalize();
	end = end.normalize();

	float cosTheta = Vector3f::dot_product(start, end);
	Vector3f rotationAxis;

	if (cosTheta < -1 + 0.001f) {
		// special case when vectors in opposite directions:
		// there is no "ideal" rotation axis
		// So guess one; any will do as long as it's perpendicular to start
		rotationAxis = Vector3f::cross_product(Vector3f(0, 0, 0), start);
		if (rotationAxis.length() < 0.01) //parallel
			rotationAxis = Vector3f::cross_product(Vector3f(1, 0, 0), start);

		rotationAxis = rotationAxis.normalize();
		return Quaternion(180, rotationAxis);
	}

	rotationAxis = Vector3f::cross_product(start, end);

	float s = std::sqrt((1 + cosTheta) * 2);
	float invS = 1 / s;

	return Quaternion(
		s * 0.5,
		rotationAxis.z * invS, //roll
		rotationAxis.y * invS, //pitch
		rotationAxis.x * invS  //yaw
	);
};