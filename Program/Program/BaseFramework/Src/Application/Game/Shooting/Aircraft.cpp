#include "Aircraft.h"
#include "Missile.h"
#include "../Scene.h"
#include "../../Component/CameraComponent.h"
#include "../../Component/InputComponent.h"
#include "../../Component/ModelComponent.h"
#include"EffectObject.h"


void Aircraft::Deserialize(const json11::Json& jsonObj)
{
	GameObject::Deserialize(jsonObj);

	if (m_spCameraComponent)
	{
		m_spCameraComponent->OffseMatrix().CreateTranslation(0.0f, 1.5f, -10.0f);
	}

	if ((GetTag() & OBJECT_TAG::TAG_Player) != 0)
	{
		Scene::GetInstance().SetTargetCamera(m_spCameraComponent);

		//プレイヤー入力
		m_spInputComponent = std::make_shared<PlayerInputComponent>(*this);

		//文字列を元にプロペラノードの検索
		KdModel::Node* propNode = m_spModelComponent->FindNode("propeller");
		if (propNode)
		{
			//プレイヤーのプロペラだけを前に進める
			propNode->m_localTransform.CreateTranslation(0.0f, 0.0f, 3.0f);
		}
		//プロペラの回転速度
		m_propRotSpeed = 0.3f;
	}
	else
	{
		//敵飛行機入力
		m_spInputComponent = std::make_shared<EnemyInputComponent>(*this);
	}

	//軌跡ポリゴン設定
	m_propTrail.SetTexture(KdResFac.GetTexture("Data/Texture/sabeline.png"));

	/*プロペラ用のGameObjectをインスタンス化
	m_spPropeller = std::make_shared<GameObject>();
	if (m_spPropeller && m_spPropeller->GetModelComponent())
	{
		//プロペラのモデルを読み込む
		m_spPropeller->GetModelComponent()->SetModel(KdResFac.GetModel("Data/Aircraft/Propeller.gltf"));

		//本体からのズレ分を格納しておく
		m_mPropLocal.CreateTranslation(0.0f, 0.0f, 2.85f);

		//プロペラのスピードを格納しておく
		m_propRotSpeed = 0.3f;
	}*/

}


void Aircraft::Update()
{

	if (m_spInputComponent)
	{
		m_spInputComponent->Update();
	}

	m_prevPos = m_mWorld.GetTranslation();

	//UpdateMove();

	//UpdateShoot();

	//UpdateCollision();

	//UpdatePropeller();//プロペラ更新

	if(m_spActionState)
	{
		m_spActionState->Update(*this);
	}

	if (m_spCameraComponent)
	{
		m_spCameraComponent->SetCameraMatrix(m_mWorld);
	}

}

void Aircraft::UpdateMove()
{
	if (m_spInputComponent == nullptr) { return; }

	const Math::Vector2& inputMove = m_spInputComponent->GetAxis(Input::Axes::L);
	//移動ベクトル作成
	KdVec3 move = { inputMove.x,0.0f,inputMove.y };

	move.Normalize();	//正規化(詳しくは授業)

	//移動速度補正
	move *= m_speed;

	//m_mWorld._41 += move.x;//41(x)
	//m_mWorld._42 += move.y;//42(y)
	//m_mWorld._43 += move.z;//43(z)

	//移動行列作成
	//Math::Matrix moveMat = DirectX::XMMatrixTranslation(move.x, move.y, move.z);
	KdMatrix moveMat;
	moveMat.CreateTranslation(move.x, move.y, move.z);

	//ワールド行列に合成
	//m_mWorld = DirectX::XMMatrixMultiply(moveMat, m_mWorld);
	m_mWorld = moveMat * m_mWorld;

	//回転ベクトル作成
	//Math::Vector3 rotate = { 0.0f,0.0f,0.0f };
	KdVec3 rotate;

	if (GetAsyncKeyState('W') & 0x8000) { rotate.x = 1.0f; }
	if (GetAsyncKeyState('A') & 0x8000) { rotate.z = -1.0f; }
	if (GetAsyncKeyState('S') & 0x8000) { rotate.x = -1.0f; }
	if (GetAsyncKeyState('D') & 0x8000) { rotate.z = 1.0f; }

	//回転行列作成
	//Math::Matrix rotateMat = DirectX::XMMatrixRotationX(rotate.x * KdToRadians);
	//rotateMat = DirectX::XMMatrixMultiply(rotateMat, DirectX::XMMatrixRotationZ(rotate.z * KdToRadians));
	KdMatrix rotateMat;
	rotateMat.CreateRotationX(rotate.x * KdToRadians);
	rotateMat.RotateZ(rotate.z * KdToRadians);

	//ワールド行列に合成
	//m_mWorld = DirectX::XMMatrixMultiply(rotateMat, m_mWorld);
	m_mWorld = rotateMat * m_mWorld;
}

void Aircraft::UpdateShoot()
{
	if (m_spInputComponent == nullptr) { return; }

	if (m_spInputComponent->GetButton(Input::Buttons::A))
	{
		if (m_canshoot)
		{
			std::shared_ptr<Missile> spMissile = std::make_shared<Missile>();

			if (spMissile)
			{
				spMissile->Deserialize(KdResFac.GetJSON("Data/Scene/Missile.json"));

				KdMatrix mLaunch;
				mLaunch.CreateRotationX((rand() % 120 - 60.0f) * KdToRadians);
				mLaunch.RotateY((rand() % 120 - 60.0f) * KdToRadians);
				mLaunch *= m_mWorld;

				spMissile->SetMatrix(mLaunch);

				spMissile->SetOwner(shared_from_this());

				Scene::GetInstance().AddObject(spMissile);

				//一番近いオブジェクトとの距離を格納する変数：初期値はfloatで最も大きな値を入れておく
				float minDistance = FLT_MAX;

				//誘導する予定のターゲットGameObjct
				std::shared_ptr<GameObject> spTarget = nullptr;

				//全ゲームオブジェクトのリストからミサイルが当たる対象を探す
				for (auto object : Scene::GetInstance().GetObjects())
				{
					//発射した飛行機自身は無視
					if (object.get() == this) { continue; }

					if ((object->GetTag() & TAG_AttacHit))
					{
						//（ターゲットの座標 - 自身の座標）の長さの2乗
						float distance = KdVec3(object->GetMatrix().GetTranslation() - m_mWorld.GetTranslation()).LenghtSquared();

						//一番近いオブジェクトとの距離よりも近ければ
						if (distance < minDistance)
						{
							//誘導する予定のターゲットを今チェックしたGameObjectに置き換え
							spTarget = object;
							//一番近いオブジェクトとの距離を今のものに更新
							minDistance = distance;
						}
					}
				}
				//誘導するターゲットのセット
				spMissile->SetTarget(spTarget);
			}
			m_canshoot = false;
		}
	}
	else
	{
		m_canshoot = true;
	}
	m_laser = (m_spInputComponent->GetButton(Input::Buttons::B) != InputComponent::FREE);
}

//当たり判定の更新
void Aircraft::UpdateCollision()
{
	if (m_laser)
	{
		//レイの発射情報
		RayInfo rayInfo;
		rayInfo.m_pos = m_prevPos;//移動する前の地点から
		rayInfo.m_dir = m_mWorld.GetAxisZ();//自分の向いてる方向に
		rayInfo.m_dir.Normalize();
		rayInfo.m_maxRange = m_laserRange;//レーザーの射程分判定

		//レイの判定結果
		KdRayResult rayResult;

		//すべてのオブジェクトと判定
		for (auto& obj : Scene::GetInstance().GetObjects())
		{
			//自分自身は無視
			if (obj.get() == this) { continue; }

			//背景タグ以外は無視
			if (!(obj->GetTag() & TAG_StageObject)) { continue; }

			//判定実行
			if (obj->HitCheckByRay(rayInfo, rayResult))
			{
				//当たったのであれば爆発をインスタンス化
				std::shared_ptr<EffectObject> effectObj = std::make_shared<EffectObject>();

				//相手の飛行機へダメージ通知
				OnNotify_Damage(5);

				if (effectObj)
				{
					//キャラクターのリストに爆発の追加
					Scene::GetInstance().AddObject(effectObj);

					//レーザーのヒット位置＝レイの発射位置＋（レイの発射方向ベクトル＊レイが当たった地点までの距離）
					KdVec3 hitPos(rayInfo.m_pos);
					hitPos = hitPos + (rayInfo.m_dir * rayResult.m_distance);

					//爆発エフェクトの行列を計算
					KdMatrix mMat;
					mMat.CreateTranslation(hitPos.x, hitPos.y, hitPos.x);
					effectObj->SetMatrix(mMat);
				}
			}
		}
	}
	//一回の移動量と移動方向を計算
	KdVec3 moveVec = m_mWorld.GetTranslation() - m_prevPos;//動く前->今の場所

	float moveDistance = moveVec.Length();//一回の移動量

	//動いていないなら判定しない
	if (moveDistance == 0.0f) { return; }

	//球情報の作成
	SphereInfo info;
	info.m_pos = m_mWorld.GetTranslation();
	info.m_radius = m_colRadiud;

	for (auto& obj : Scene::GetInstance().GetObjects())
	{
		//自分自身を無視
		if (obj.get() == this) { continue; }

		//キャラクターと当たり判定をするのでそれ以外は無視
		if (!(obj->GetTag() & TAG_Cheracter)) { continue; }

		//当たり判定
		if (obj->HitCheckBySphere(info))
		{
			Scene::GetInstance().AddDebugSphereLine
			(m_mWorld.GetTranslation(), 20.f, { 1.0f,0.0f,0.0f,1.0f });

			//移動する前の位置に戻る
			m_mWorld.SetTranslation(m_prevPos);
		}
	}

	//レイによる当たり判定
	//レイ情報の作成
	RayInfo rayInfo;
	rayInfo.m_pos = m_prevPos;			//1つ前の場所から
	rayInfo.m_dir = moveVec;			//動いた方向に向かって
	rayInfo.m_maxRange = moveDistance;	//動いた分だけ判定を行う

	rayInfo.m_dir.Normalize();

	KdRayResult rayResult;

	for (auto& obj : Scene::GetInstance().GetObjects())
	{
		//自分自身は無視
		if (obj.get() == this) { continue; }

		//背景タグ以外は無視
		if (!(obj->GetTag() & TAG_StageObject)) { continue; }

		//判定実行
		if (obj->HitCheckByRay(rayInfo, rayResult))
		{
			//移動する前の1フレーム前に戻る
			m_mWorld.SetTranslation(m_prevPos);
		}
	}
}

void Aircraft::UpdatePropeller()
{
	KdModel::Node* propNode = m_spModelComponent->FindNode("propeller");
	if (propNode)
	{
		//ローカル行列の回転
		propNode->m_localTransform.RotateZ(m_propRotSpeed);

		//プロペラの中心座標(World)
		KdMatrix propCenterMat;
		propCenterMat *= propNode->m_localTransform * m_mWorld;

		//プロペラの外側座標(World)
		KdMatrix propOuterMat;
		//そこからY軸へ少しずらした位置(モデルのスケールが変わると通用しない)
		propOuterMat.CreateTranslation(0.0f, 1.8f, 0.0f);
		propOuterMat *= propCenterMat;

		//Strip描画するため2つで1ペア追加
		m_propTrail.AddPoint(propCenterMat);
		m_propTrail.AddPoint(propOuterMat);

		//30個より多く登録されていたら
		if (m_propTrail.GetNumPoints() > 30)
		{
			//Strip描画するため2つで1ペア消す
			m_propTrail.DelPoint_Back();
			m_propTrail.DelPoint_Back();
		}
	}
}

void Aircraft::OnNotify_Damage(int damage)
{
	m_hp -= damage;//相手の攻撃カ分、HPを減らす

	//HPが０になったら
	if (m_hp <= 0)
	{
		m_spActionState = std::make_shared<ActionFly>();
	}
}

void Aircraft::Draw()
{
	GameObject::Draw();//基底クラスのDrawを呼び出す

	//レーザー描画
	if (m_laser)
	{
		//レーザーの終点を求める
		KdVec3 laserStart(m_prevPos);
		KdVec3 laserEnd;
		KdVec3 laserDir(m_mWorld.GetAxisZ());

		laserDir.Normalize();//拡大が入っていると1以上になるので正規化

		laserDir *= m_laserRange;//レーザーの射程分方向ベクトルを伸ばす

		laserEnd = laserStart * laserDir;//レーザーの終点は発射位置ベクトル＋レーザーの長さ分

		Scene::GetInstance().AddDebugLine(m_prevPos, laserEnd, { 0.0f,1.0f,1.0f,1.0f });
	}
}

void Aircraft::DrawEffect()
{
	D3D.GetDevContext()->OMSetBlendState(SHADER.m_bs_Add, Math::Color(0, 0, 0, 0), 0xFFFFFFFF);

	SHADER.m_effectShader.SetWorldMatrix(KdMatrix());
	SHADER.m_effectShader.WriteToCB();

	m_propTrail.DrawStrip();

	D3D.GetDevContext()->OMSetBlendState(SHADER.m_bs_Alpha, Math::Color(0, 0, 0, 0), 0xFFFFFFFF);
}

void Aircraft::ActionFly::Update(Aircraft& owner)
{
	owner.UpdateMove();

	owner.UpdateCollision();

	owner.UpdateShoot();
}

void Aircraft::ActionCrash::Update(Aircraft& owner)
{
	if (!(--m_timer))
	{
		owner.Destroy();
	}

	KdMatrix rotation;
	rotation.CreateRotationX(0.08f);
	rotation.RotateY(0.055f);
	rotation.RotateZ(0.03f);

	owner.m_mWorld = rotation * owner.m_mWorld;

	owner.m_mWorld.Move(KdVec3(0.0f, -0.2f, 0.0f));
}
