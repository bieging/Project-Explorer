<br>
### Issues annotations
<br>
# Issue #15
<br>

Task 1
<br>
Check if space key was released. If so, start decreasing the gravityVelocity until it reaches zero
Once the gravityVelocity reaches zero, let the gravity act

<br>

```cpp
bool spaceReleased = false;

void updatePlayerVelocity(GLfloat dt)
{
	if (spacePressed == false)
	{
		if (spaceReleased)
		{
			if (gravityVelocity >= 0)
			{
				gravityVelocity -= 0.5 * dt;
				playerPos.y += gravityVelocity;
			}
			else
			{
				spaceReleased = false;
				gravityVelocity = 0;
			}
		}
		else
		{
			//.....
		}
	}
	else
	{
		//.....

		spaceReleased = true;
	}
}
```
<br>
Task 2
<br>

Create timer that start decreasing once the player begins the jump
Once the counter gets to 0, stop the player jump by setting spacePressed to false and spaceReleased to true
Reset counter when condition is met

<br>

```cpp
int jumpTime = 7;
int jumpTimeCounter = jumpTime;

void updatePlayerVelocity(GLfloat dt)
{
	if (spacePressed == false)
	{
		//.....
	}
	else
	{
		//.....

		jumpTimeCounter -= dt;

		if (jumpTimeCounter <= 0)
		{
			spacePressed = false;
			spaceReleased = true;

			jumpTimeCounter = jumpTime;
		}
	}
}
			

if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
{
	if (jumpEnable)
	{
		spacePressed = true;
		jumpEnable = false;
	}
}
```

<br>
Task 3
<br>
Create var jumpEnable, and set it to false once the player presses space, and only set spacePressed = true if jumpEnable = true

<br>

```cpp
bool jumpEnable = true;

void updatePlayerVelocity(GLfloat dt)
{
	if (spacePressed == false)
	{
		if (spaceReleased)
		{
			//.....
		}
		else
		{
			//.....

			if (playerPos.y > heightValue.at(heightI))
			{
				//.....
			}
			else
			{
				playerPos.y = heightValue.at(heightI);

				gravityVelocity = 0.05f;

				jumpEnable = true;
			}
		}
	}
	else
	{
		//.....
	}
}
			

if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
{
	if (jumpEnable)
	{
		spacePressed = true;
		jumpEnable = false;
	}
}
```

<br>

<br>
# Issue #13
<br>

Task 1

<br>

Jump must be disabled once free-flight is enabled
Jump must be used and do ascend and descend on locked-height-flight. Space to ascend and Shift + Space to descend
Created PLAYER_GRAVITY_STATE enum to control if player if falling, fixed-flying or free-flying

<br>

```cpp
Code
```

<br>