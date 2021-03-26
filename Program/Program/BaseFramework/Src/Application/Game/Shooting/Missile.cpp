#include"Missile.h"
#include "Application/main.h"
#include "../../Component/ModelComponent.h"
#include"../Scene.h"
#include "AnimationEffect.h"


void Missile::Deserialize(const json11::Json& jsonObj)
{
	m_lifeSpan = APP.m_maxFps * 10;

	if (jsonObj.is_null() == false) { return; }

	GameObject::Deserialize(jsonObj);

	if (jsonObj["Speed"].is_null() == false)
	{
		m_speed = jsonObj["Speed"].number_value();
	}

	//煙テクスチャ
	m_trailSmoke.SetTexture(KdResFac.GetTexture("Data/Texture/smokeline2.png"));
}

void Missile::Update()
{
	if (m_alive == false) { return; }

	//寿命
	m_prevPos = m_mWorld.GetTranslation();

	//移動
	UpdateCollision();

	if (--m_lifeSpan <= 0)
	{
		Destroy();
	}

	//ターゲットをshared_ptr化
	auto target = m_wpTarget.lock();

	if (target)
	{

		//自分自身からターゲットへのベクトル
		KdVec3 vTarget = target->GetMatrix().GetTranslation() - m_mWorld.GetTranslation();

		//単位ベクトル化：自身からターゲットへ向かう長さ1のベクトル
		vTarget.Normalize();

		//自分のZ方向（前方向）
		KdVec3 vZ = m_mWorld.GetAxisZ();

		//拡大率が入っていると計算がおかしくなるため単位ベクトル化
		vZ.Normalize();

		//=========回転軸作成（この軸で回転する）======
		KdVec3 vRotAxis = KdVec3::Cross(vZ, vTarget);

		//0ベクトルなら回転しない
		if (vRotAxis.LenghtSquared() != 0)
		{
			//じぶんのZ 方向ベクトルと自信からターゲットへ向かうベクトルの内積
			float d = KdVec3::Dot(vZ, vTarget);

			//誤差で-1～1以外になる可能性大なので、クランプする
			if (d > 1.0f)d = 1.0f;
			else if (d < -1.0f)d = -1.0f;

			//自分の前方向ベクトルと自身からターゲットへ向かうベクトル間の角度（radian）
			float radian = acos(d);

			//角度制限、1フレームにつき最大で1度以上回転しない
			if (radian > 1.0f * KdToRadians)
			{
				radian = 1.0f * KdToRadians;
			}
			//=====radian（ここまでで回転角度が求まった）=====

			KdMatrix mRot;
			mRot.CreateRotetionAxis(vRotAxis, radian);
			auto pos = m_mWorld.GetTranslation();
			m_mWorld.SetTranslation({ 0,0,0 });
			m_mWorld *= mRot;
			m_mWorld.SetTranslation(pos);
		}
	}
	KdVec3 move = m_mWorld.GetAxisZ();
	move.Normalize();

	move *= m_speed;

	m_mWorld.Move(move);

	//軌跡の更新
	UpdateTrail();

}

#include"Aircraft.h"
void Missile::UpdateCollision()
{
	//発射した主人のshared_ptr取得
	KdVec3 moveVec = m_mWorld.GetTranslation() - m_prevPos;

	float moveDistance = moveVec.Length();

	//動いていないなら判定しない
	if (moveDistance == 0.0f) { return; }

	//球判定情報を作成
	SphereInfo info;
	info.m_pos = m_mWorld.GetTranslation();
	info.m_radius = m_colRadiud;

	auto spOwner = m_wpOwner.lock();

	for (auto& obj : Scene::GetInstance().GetObjects())
	{
		//自分自身は無視
		if (obj.get() == this) { continue; }

		//発射した主人も無視
		if (obj.get() == spOwner.get()) { continue; }

		bool hit = false;

		//キャラクターが対象
		//TAG_Characterとは球判定を行う
		if (!(obj->GetTag() & TAG_Cheracter))
		{
			hit = obj->HitCheckBySphere(info);

			if (hit)
			{
				//dynamic_pointer_cast = 基底クラス型をダウンキャストするときに使う、失敗するとnullptrが返る
				//重たい、多発する場合は設計がミスっている
				std::shared_ptr<Aircraft> aircraft = std::dynamic_pointer_cast<Aircraft>(obj);
				if (aircraft)
				{
					aircraft->OnNotify_Damage(m_attackPow);
				}
			}
		}
		if (hit)
		{
			Explosion();
			Destroy();
		}
	}

	//TAG_StageObjectとはレイ判定を行う
	RayInfo rayInfo;
	rayInfo.m_pos = m_prevPos;
	rayInfo.m_dir = moveVec;
	rayInfo.m_dir.Normalize();

	KdRayResult rayResult;

	for (auto& obj : Scene::GetInstance().GetObjects())
	{
		//自分は無視
		if (obj.get() == this) { continue; }

		//背景オブジェクトが対象
		if (!(obj->GetTag() & TAG_StageObject)) { continue; }

		if (obj->HitCheckByRay(rayInfo,rayResult))
		{
			Destroy();
		}
	}

}

void Missile::Explosion()
{
	//アニメーションエフェクトをインスタンス化
	std::shared_ptr<AnimationEffect> effect = std::make_shared<AnimationEffect>();

	//爆発のテクスチャとアニメーション情報を渡す
	effect->SetAnimationInfo(
		KdResFac.GetTexture("Data/Texture/Explosion00.png"), 10.0f, 5, 5, rand() % 360);

	//場所をミサイル（自分）の位置に合わせる
	effect->SetMatrix(m_mWorld);

	//リストに追加
	Scene::GetInstance().AddObject(effect);
}

void Missile::UpdateTrail()
{

	//軌跡の座標を先頭に追加
	m_trailSmoke.AddPoint(m_mWorld);

	//軌跡の数限定（100以前の軌跡を消去する）
	if (m_trailSmoke.GetNumPoints() > 100)
	{
		m_trailSmoke.DelPoint_Back();
	}
}

void Missile::DrawEffect()
{
	if (!m_alive) { return; }

	SHADER.m_effectShader.SetWorldMatrix(KdMatrix());

	SHADER.m_effectShader.WriteToCB();
	
	m_trailSmoke.DrawBillboard(0.5f);
}


