extends CharacterBody2D

var speed = 200.0

func _physics_process(delta):
	var _velocity = Vector2.ZERO

	if Input.is_action_pressed("right"):
		_velocity.x += 1
	if Input.is_action_pressed("left"):
		_velocity.x -= 1
	if Input.is_action_pressed("down"):
		_velocity.y += 1
	if Input.is_action_pressed("up"):
		_velocity.y -= 1

	_velocity = velocity.normalized()
	
	velocity = _velocity * speed

	move_and_slide()
