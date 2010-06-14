#include "LifeForm.h"

violetland::LifeForm::LifeForm(float x, float y, int w, int h) :
	Object(x, y, w, h) {
	unsigned long t;
#ifdef _WIN32
	SYSTEMTIME winT;
	GetLocalTime(&winT);
	FILETIME fT;
	SystemTimeToFileTime(&winT, &fT);
	ULARGE_INTEGER sec;
	sec.HighPart = fT.dwHighDateTime;
	sec.LowPart = fT.dwLowDateTime;
	t = sec.QuadPart / 1E7;
#endif //_WIN32
#if defined linux || defined __FreeBSD__ || defined __APPLE__
	t = static_cast<unsigned long> (time(NULL));
#endif  //linux || __FreeBSD__ || __APPLE__
	char buf[30];
	sprintf(buf, "%09i-%09li", (rand() % 999999999), t);
	Id = buf;
	Strength = 1.0f;
	Agility = 1.0f;
	Vitality = 1.0f;
	m_health = MaxHealth();
	State = LIFEFORM_STATE_ALIVE;
	m_lastAttackTime = SDL_GetTicks();
	TargetX = TargetY = 0.0f;
	Poisoned = false;
	Frozen = 0;
	Level = 1;
	Name = "Unknown lifeform";
}

void violetland::LifeForm::draw() {
	// nothing
}

void violetland::LifeForm::hit(float damage, bool poison, float pX, float pY) {
	setHealth(getHealth() - damage);
}

void violetland::LifeForm::process(int deltaTime) {
	if (Frozen > 0) {
		Frozen -= deltaTime;
		if (Frozen < 0)
			Frozen = 0;
	}

	if (getHealth() == 0 && State == LIFEFORM_STATE_ALIVE) {
		State = LIFEFORM_STATE_SMITTEN;
	}

	if (State == LIFEFORM_STATE_ALIVE && Frozen == 0) {
		setHealth(getHealth() + HealthRegen() * deltaTime);

		if (Poisoned)
			setHealth(getHealth() - 0.0002 * deltaTime);
	}
}

StaticObject* violetland::LifeForm::getCorpse() {
	return NULL;
}

float violetland::LifeForm::getStrength() {
	return Strength;
}
float violetland::LifeForm::getAgility() {
	return Agility;
}

float violetland::LifeForm::getVitality() {
	return Vitality;
}
const float violetland::LifeForm::MaxHealth() {
	return getVitality() > 0.8f ? 1.0f + (getVitality() - 1.0f) * 2.0f
			+ (getStrength() - 1.0f) : 0.4f;
}

const float violetland::LifeForm::ChanceToEvade() {
	return getAgility() > 1.0f ? (getAgility() - 1.0f) / 2.0f : 0.0f;
}

const bool violetland::LifeForm::Attack() {
	int now = SDL_GetTicks();

	if (now - m_lastAttackTime > AttackDelay()) {
		m_lastAttackTime = now;
		return true;
	} else {
		return false;
	}
}

const float violetland::LifeForm::Damage() {
	return getStrength() / 8.0f;
}

const int violetland::LifeForm::AttackDelay() {
	return (1.0f - (getAgility() - 1.0f) / 2.0f) * 1000;
}

const float violetland::LifeForm::MaxSpeed() {
	return getAgility() / 5.0f;
}

const float violetland::LifeForm::HealthRegen() {
	return getVitality() > 1.0f ? (getVitality() - 1.0f) * 0.000005f : 0.0f;
}

const float violetland::LifeForm::ReloadSpeedMod() {
	return 1.0f / getAgility();
}

const float violetland::LifeForm::WeaponRetForceMod() {
	return getStrength() > 1.0f ? 1.0f - (getStrength() - 1.0f) * 1.1f : 1.0f;
}

void violetland::LifeForm::setHealth(float value) {
	m_health = value;
	if (m_health > MaxHealth())
		m_health = MaxHealth();
	if (m_health < 0)
		m_health = 0;
}

const float violetland::LifeForm::getHealth() {
	return m_health;
}
