export default function Inventory(props)
{
  const styles = {
    position: "absolute",
    top: "85%",
    left: "85%",
    zIndex: "10"
  }
  const names = {
    1: "Stone",
    2: "Bricks",
    3: "Sand",
    4: "Grass"
  }
  return (
    <div style={styles}>
      <p>{props.id} ({names[`${props.id}`]})</p>
    </div>
  );
}