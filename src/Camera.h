#ifndef __CAMERA
#define __CAMERA

class Camera
{
public:
	Camera()
	{ }

	Camera(Vector3 const & pos, Vector3 const & focusPoint, Vector3 const & up, Vector3 const & motion)
	{
		setBasis(pos, focusPoint, up);
		this->pos = pos;
		this->motion = motion;
		setFov(90.0f / 180.0f * M_PI);
	}

	void setBasis(Vector3 const & pos, Vector3 const & focusPoint, Vector3 const & up)
	{
		Vector3 look = (focusPoint - pos).normalize();
		this->w = -look;
		this->u = up.cross(w).normalize();
		this->v = w.cross(u);
	}

	Vector3 getU() const { return this->u; }
	Vector3 getV() const { return this->v; }
	Vector3 getW() const { return this->w; }
	Vector3 getPos() const { return this->pos; }
	Vector3 getMotion() const { return this->motion; }
	float getFov() const { return this->fov; }

	void setFov(float const & fov)
	{ this->fov = fov; }

private:

	Vector3 pos, u, v, w, motion;
	float fov;
	float aspectRatio;
};

#endif

