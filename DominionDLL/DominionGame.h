
class DominionGame
{
public:
    void NewGame(const Vector<PlayerInfo> &playerList, const GameOptions &options);
    void Init(const CardDatabase &cards);
    __forceinline State& state()
    {
        return _state;
    }
    __forceinline const GameData& data() const
    {
        return _data;
    }

private:
    GameData _data;
    State _state;
};