#pragma once

class particles
{ // a particle in itself would need a SHAPE, a LIFETIME and a COLOR - mayb tracks its own SPEED for variations
public: 
	XMFLOAT3 pos;			// Starting point POSITION
	XMFLOAT3 dir;			// DIRECTION of the particles
	XMFLOAT3 radius;		// size thing

	int quantity;			// Amount of particles emitted per second?
	float speed;			// SPEED of the particles
};

