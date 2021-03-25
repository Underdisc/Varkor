#version 330 core

out int iMemberId;

uniform int uMemberId;

void main()
{
  iMemberId = uMemberId;
}

